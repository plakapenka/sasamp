package com.liverussia.launcher.async;

import android.content.Context;
import android.os.PowerManager;
import android.view.View;
import android.widget.ProgressBar;

import androidx.annotation.UiThread;
import androidx.annotation.WorkerThread;

import com.google.firebase.crashlytics.FirebaseCrashlytics;
import com.liverussia.cr.core.DownloadUtils;
import com.liverussia.launcher.activity.LoaderActivity;
import com.liverussia.launcher.async.domain.AsyncTaskResult;
import com.liverussia.launcher.async.listener.OnAsyncCriticalErrorListener;
import com.liverussia.launcher.async.listener.OnAsyncSuccessListener;
import com.liverussia.launcher.dto.response.FileInfo;
import com.liverussia.launcher.dto.response.GameFileInfoDto;
import com.liverussia.launcher.dto.response.LatestVersionInfoDto;
import com.liverussia.launcher.enums.DownloadType;
import com.liverussia.launcher.error.apiException.ApiException;
import com.liverussia.launcher.error.apiException.ErrorContainer;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;
import com.techyourchance.threadposter.BackgroundThreadPoster;
import com.techyourchance.threadposter.UiThreadPoster;

import org.apache.commons.lang3.StringUtils;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpMethod;
import org.springframework.http.ResponseEntity;
import org.springframework.http.converter.json.MappingJackson2HttpMessageConverter;
import org.springframework.web.client.RestTemplate;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.SocketException;
import java.net.URI;
import java.net.URL;
import java.net.UnknownHostException;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Optional;
import java.util.Set;
import java.util.concurrent.atomic.AtomicLong;

import javax.net.ssl.SSLException;

import static com.liverussia.launcher.config.Config.FILE_INFO_URL;

public class DownloadTask implements Listener<TaskStatus> {

    private final LoaderActivity loaderActivity;
    private final ProgressBar progressBar;
    private final ActivityService activityService;

    private OnAsyncSuccessListener onAsyncSuccessListener;
    private OnAsyncCriticalErrorListener onAsyncCriticalErrorListener;

    private PowerManager.WakeLock mWakeLock;

    private long fileLengthFull;
    private AtomicLong fileLengthMin;
    private long total;
    private Set<FileInfo> files;

    private final UiThreadPoster uiThreadPoster;
    private final BackgroundThreadPoster backgroundThreadPoster;

    public DownloadTask(LoaderActivity loaderActivity, ProgressBar progressBar) {
        this.loaderActivity = loaderActivity;
        this.progressBar = progressBar;
    }

    {
        activityService = new ActivityServiceImpl();
        uiThreadPoster = new UiThreadPoster();
        backgroundThreadPoster = new BackgroundThreadPoster();
    }

    public void download() {
        configureProgressBar();
        backgroundThreadPoster.post(this::downloadGameFiles);
    }

    public void reloadCache() {
        configureProgressBar();
        backgroundThreadPoster.post(this::reloadGameFiles);
    }

    public void updateApk() {
        configureProgressBar();
        backgroundThreadPoster.post(this::downloadApk);
    }

    @WorkerThread
    private void downloadApk() {
        LatestVersionInfoDto apkInfo = DownloadUtils.LATEST_APK_INFO;
        FileInfo fileInfo = buildFileInfoForApkFile(apkInfo);

        List<FileInfo> filesInfo = new ArrayList<>();
        filesInfo.add(fileInfo);

        fileLengthFull = apkInfo.getSize();
        fileLengthMin = new AtomicLong(0);
        total = 0;

        files = new HashSet<>(filesInfo);

        try {
            downloadAndSaveFiles(filesInfo);
        } catch (ApiException e) {
            uiThreadPoster.post(() -> {
                onAsyncFinished(new AsyncTaskResult<>(e));
            });
        } catch (Exception e) {
            uiThreadPoster.post(() -> {
                onAsyncFinished(new AsyncTaskResult<>(new ApiException(ErrorContainer.DOWNLOAD_FILES_ERROR)));
            });
        }
    }

    private FileInfo buildFileInfoForApkFile(LatestVersionInfoDto apkInfo) {
        FileInfo fileInfo = new FileInfo();
        fileInfo.setLink(apkInfo.getLink());
        fileInfo.setSize(apkInfo.getSize());
        fileInfo.setPath(apkInfo.getPath());

        return fileInfo;
    }

    @WorkerThread
    private void reloadGameFiles() {
        List<FileInfo> filesInfo = DownloadUtils.FILES_TO_RELOAD;
        fileLengthFull = calculateSize(filesInfo);
        fileLengthMin = new AtomicLong(0);
        total = 0;
        files = new HashSet<>(filesInfo);

        try {
            downloadAndSaveFiles(filesInfo);
        } catch (ApiException e) {
            uiThreadPoster.post(() -> {
                onAsyncFinished(new AsyncTaskResult<>(e));
            });
        } catch (Exception e) {
            uiThreadPoster.post(() -> {
                onAsyncFinished(new AsyncTaskResult<>(new ApiException(ErrorContainer.DOWNLOAD_FILES_ERROR)));
            });
        }
    }

    @WorkerThread
    private void downloadGameFiles() {
        try {
            GameFileInfoDto gameFilesInfo = getGameFilesInfo();

            fileLengthFull = gameFilesInfo.getAllFilesSize();
            fileLengthMin = new AtomicLong(0);
            total = 0;

            files = new HashSet<>(gameFilesInfo.getFiles());

            downloadAndSaveFiles(gameFilesInfo.getFiles());
        } catch (ApiException e) {
            uiThreadPoster.post(() -> {
                onAsyncFinished(new AsyncTaskResult<>(e));
            });
        } catch (Exception e) {
            uiThreadPoster.post(() -> {
                onAsyncFinished(new AsyncTaskResult<>(new ApiException(ErrorContainer.DOWNLOAD_FILES_ERROR)));
            });
        }
    }

    @WorkerThread
    private void downloadAndSaveFilesAfterFailed(List<FileInfo> gameFilesInfo) {
        try {
            Optional.ofNullable(gameFilesInfo)
                    .orElse(Collections.emptyList())
                    .forEach(this::downloadAndSaveFile);

            uiThreadPoster.post(() -> {
                onAsyncFinished(new AsyncTaskResult<>(TaskStatus.SUCCESS));
            });
        } catch (ApiException e) {
            uiThreadPoster.post(() -> {
                onAsyncFinished(new AsyncTaskResult<>(e));
            });
        } catch (Exception e) {
            uiThreadPoster.post(() -> {
                onAsyncFinished(new AsyncTaskResult<>(new ApiException(ErrorContainer.DOWNLOAD_FILES_ERROR)));
            });
        }
    }

    private long calculateSize(List<FileInfo> filesInfo) {
        return Optional.ofNullable(filesInfo)
                .orElse(Collections.emptyList())
                .stream()
                .mapToLong(FileInfo::getSize)
                .sum();
    }

    private GameFileInfoDto getGameFilesInfo() {
        URI url = URI.create(FILE_INFO_URL);

        RestTemplate restTemplate = new RestTemplate();
        restTemplate.getMessageConverters().add(new MappingJackson2HttpMessageConverter());
        ResponseEntity<GameFileInfoDto> response = restTemplate.exchange(url, HttpMethod.GET, HttpEntity.EMPTY, GameFileInfoDto.class);

        return response.getBody();
    }

    private void downloadAndSaveFiles(List<FileInfo> gameFilesInfo) {
        Optional.ofNullable(gameFilesInfo)
                .orElse(Collections.emptyList())
                .forEach(this::downloadAndSaveFile);

        uiThreadPoster.post(() -> {
            onAsyncFinished(new AsyncTaskResult<>(TaskStatus.SUCCESS));
        });

    }

    private void downloadAndSaveFile(FileInfo fileInfo) {
        InputStream input = null;
        OutputStream output = null;
        HttpURLConnection connection = null;
        long fileLoadedSize = 0;
        File file = null;

        try {
            URL url = new URL(fileInfo.getLink());
            connection = (HttpURLConnection) url.openConnection();
            connection.connect();

            if (connection.getResponseCode() != HttpURLConnection.HTTP_OK) {
                throw new ApiException(ErrorContainer.DOWNLOAD_FILES_ERROR);
            }

            long fileLength = connection.getContentLengthLong();
            input = connection.getInputStream();

            file = new File(loaderActivity.getExternalFilesDir(null).toString()
                    .concat("/")
                    .concat(fileInfo.getPath().replace("files/", ""))
            );

            if (file.exists()) {
                file.delete();
            }

            file.getParentFile().mkdirs();

            output = new FileOutputStream(loaderActivity
                    .getExternalFilesDir(null).toString()
                    .concat("/")
                    .concat(fileInfo.getPath().replace("files/", ""))
            );

            byte[] data = new byte[4096];
            int count;

            while ((count = input.read(data)) != -1) {
//                if (isCancelledDownload()) {
//                    input.close();
//                    throw new ApiException(ErrorContainer.DOWNLOAD_WAS_INTERRUPTED);
//                }

                total += count;
                fileLoadedSize += count;

                if (fileLength > 0) {
                    uiThreadPoster.post(() -> {
                        if (fileLengthFull == 0) {
                            fileLengthFull += 1;
                        }
                        publishProgress((int) (total * 100 / fileLengthFull));
                    });
                }

                fileLengthMin.set(total);

                output.write(data, 0, count);
            }

            files.remove(fileInfo);
        } catch (ApiException e) {
            if (file != null && file.exists()) {
                file.delete();
            }

            throw new ApiException(e);
        } catch (UnknownHostException | SocketException | SSLException e) {
            boolean isDeleted = false;

            if (file != null && file.exists()) {
                isDeleted = file.delete();
            }

            if (isDeleted) {
                total -= fileLoadedSize;
            }

            throw new ApiException(ErrorContainer.INTERNET_WAS_DISABLE);
        } catch (Exception e) {
            if (file != null && file.exists()) {
                file.delete();
            }

            FirebaseCrashlytics.getInstance().recordException(e);
            //TODO исправить
            throw new ApiException(ErrorContainer.DOWNLOAD_FILES_ERROR, e.toString());
        } finally {
            try {
                if (output != null)
                    output.close();
                if (input != null)
                    input.close();
            } catch (IOException ignored) {
            }

            if (connection != null)
                connection.disconnect();
        }
    }

    @UiThread
    private void publishProgress(int progress) {
        if (DownloadType.LOAD_ALL_CACHE.equals(DownloadUtils.getType()) || DownloadType.RELOAD_OR_ADD_PART_OF_CACHE.equals(DownloadUtils.getType())) {
            loaderActivity.getLoading().setText("Загрузка файлов игры...");
            loaderActivity.getLoadingPercent().setText(progress + "%");
            loaderActivity.getFileName().setText(formatFileSize(fileLengthMin.longValue())+" из "+formatFileSize(fileLengthFull));
        }

        if (DownloadType.UPDATE_APK.equals(DownloadUtils.getType())) {
            loaderActivity.getLoading().setText("Обновление...");
            loaderActivity.getLoadingPercent().setText(progress + "%");
            loaderActivity.getFileName().setText(formatFileSize(fileLengthMin.longValue())+" из "+formatFileSize(fileLengthFull));
        }

        progressBar.setProgress(progress);
    }

    @Override
    @UiThread
    public void onAsyncFinished(AsyncTaskResult<TaskStatus> result) {

        if (isInternetDisableException(result.getException())) {
            loaderActivity.getRepeatLoadButton().setVisibility(View.VISIBLE);
            loaderActivity.getLoading().setText("Ошибка соединения...");
            return;
        }

        mWakeLock.release();
        loaderActivity.getLoading().setVisibility(View.INVISIBLE);
        loaderActivity.getLoadingPercent().setVisibility(View.INVISIBLE);
        progressBar.setVisibility(View.INVISIBLE);

        if (result.getException() != null) {
            ApiException apiException = result.getException();

            //TODO вернуть как было см чекер или коммиты
            activityService.showBigMessage(apiException.getMessage(), loaderActivity);
//            showErrorMessage(apiException);
            onAsyncErrorDo();
//        } else if (isCancelled()) {
//            showErrorMessage(new ApiException(ErrorContainer.DOWNLOAD_WAS_INTERRUPTED));
        } else if (onAsyncSuccessListener != null) {
            onAsyncSuccessListener.onSuccess();
        }
    }

    private boolean isInternetDisableException(ApiException exception) {
        return Optional.ofNullable(exception)
                .map(ApiException::getError)
                .filter(ErrorContainer.INTERNET_WAS_DISABLE::equals)
                .isPresent();
    }

    private void showErrorMessage(ApiException exception) {
        String errorMessage = Optional.ofNullable(exception)
                .map(ApiException::getError)
                .map(ErrorContainer::getMessage)
                .orElse(StringUtils.EMPTY);

        activityService.showMessage(errorMessage, loaderActivity);
    }

    private void onAsyncErrorDo() {
        if (onAsyncCriticalErrorListener != null) {
            onAsyncCriticalErrorListener.onCriticalError();
        }
    }


    public void setOnAsyncSuccessListener(OnAsyncSuccessListener onClickListener) {
        this.onAsyncSuccessListener = onClickListener;
    }

    public void setOnAsyncCriticalErrorListener(OnAsyncCriticalErrorListener onAsyncCriticalErrorListener) {
        this.onAsyncCriticalErrorListener = onAsyncCriticalErrorListener;
    }


    public static String formatFileSize(long size) {
        String hrSize = null;

        double b = size;
        double k = size/1024.0;
        double m = ((size/1024.0)/1024.0);
        double g = (((size/1024.0)/1024.0)/1024.0);
        double t = ((((size/1024.0)/1024.0)/1024.0)/1024.0);

        DecimalFormat dec = new DecimalFormat("0.00");

        if ( t>1 ) {
            hrSize = dec.format(t).concat(" TB");
        } else if ( g>1 ) {
            hrSize = dec.format(g).concat(" GB");
        } else if ( m>1 ) {
            hrSize = dec.format(m).concat(" MB");
        } else if ( k>1 ) {
            hrSize = dec.format(k).concat(" KB");
        } else {
            hrSize = dec.format(b).concat(" Bytes");
        }

        return hrSize;
    }

    private void configureProgressBar() {
        PowerManager pm = (PowerManager) loaderActivity.getSystemService(Context.POWER_SERVICE);
        mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, getClass().getName());
        mWakeLock.acquire();
        progressBar.setVisibility(View.VISIBLE);
        progressBar.setIndeterminate(false);
        progressBar.setMax(100);
        loaderActivity.getLoading().setText("Загрузка файлов игры...");
    }

    public void repeatLoad() {
        loaderActivity.getRepeatLoadButton().setVisibility(View.INVISIBLE);
        backgroundThreadPoster.post(() -> downloadAndSaveFilesAfterFailed(new ArrayList<>(files)));
    }
}
