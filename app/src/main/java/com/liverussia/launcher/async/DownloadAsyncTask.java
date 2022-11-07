package com.liverussia.launcher.async;

import android.content.Context;
import android.os.AsyncTask;
import android.os.PowerManager;
import android.util.Log;
import android.view.View;
import android.widget.ProgressBar;

import com.liverussia.cr.core.Utils;
import com.liverussia.launcher.activity.LoaderActivity;
import com.liverussia.launcher.async.domain.AsyncTaskResult;
import com.liverussia.launcher.async.listener.OnAsyncCriticalErrorListener;
import com.liverussia.launcher.async.listener.OnAsyncSuccessListener;
import com.liverussia.launcher.dto.response.FileInfo;
import com.liverussia.launcher.dto.response.GameFileInfoDto;
import com.liverussia.launcher.error.apiException.ApiException;
import com.liverussia.launcher.error.apiException.ErrorContainer;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;

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
import java.net.URI;
import java.net.URL;
import java.text.DecimalFormat;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Optional;
import java.util.concurrent.atomic.AtomicLong;
import java.util.stream.Collectors;

import static com.liverussia.launcher.config.Config.FILE_INFO_URL;
import static com.liverussia.launcher.config.Config.GAME_PATH;
import static com.liverussia.launcher.config.Config.PATH_DOWNLOADS;

public class DownloadAsyncTask extends AsyncTask<String, Integer, AsyncTaskResult<AsyncTask.Status>> {

    private final LoaderActivity loaderActivity;
    private final ProgressBar progressBar;
    private final ActivityService activityService;

    private OnAsyncSuccessListener onAsyncSuccessListener;
    private OnAsyncCriticalErrorListener onAsyncCriticalErrorListener;

    private PowerManager.WakeLock mWakeLock;

    private long fileLengthFull;
    private AtomicLong fileLengthMin;
    private long total;
    private boolean isCancel;


    public DownloadAsyncTask(LoaderActivity loaderActivity, ProgressBar progressBar) {
        super();
        this.loaderActivity = loaderActivity;
        this.progressBar = progressBar;
    }

    {
        activityService = new ActivityServiceImpl();
        this.isCancel = false;
    }

    @Override
    protected AsyncTaskResult<AsyncTask.Status> doInBackground(String... sUrl) {

        if (Utils.getType() == 0) {
            return downloadGameFiles();
        }

        if (Utils.getType() == 1) {
            return downloadLauncher(sUrl[0]);
        }

        return new AsyncTaskResult<>(new ApiException(ErrorContainer.OTHER));
    }

    private AsyncTaskResult<AsyncTask.Status> downloadGameFiles() {
        try {
            GameFileInfoDto gameFilesInfo = getGameFilesInfo();

            fileLengthFull = gameFilesInfo.getAllFilesSize();
            fileLengthMin = new AtomicLong(0);
            total = 0;

            downloadAndSaveFiles(gameFilesInfo.getFiles());
        } catch (ApiException e) {
            return new AsyncTaskResult<>(e);
        } catch (Exception e) {
            return new AsyncTaskResult<>(new ApiException(ErrorContainer.DOWNLOAD_FILES_ERROR));
        }


        return new AsyncTaskResult<>(AsyncTask.Status.FINISHED);
    }

    private void downloadAndSaveFiles(List<FileInfo> gameFilesInfo) {
        Optional.ofNullable(gameFilesInfo)
                .orElse(Collections.emptyList())
                .forEach(this::downloadAndSaveFile);
    }

    private void downloadAndSaveFile(FileInfo fileInfo) {
        InputStream input = null;
        OutputStream output = null;
        HttpURLConnection connection = null;

        try {
            URL url = new URL(fileInfo.getLink());
            connection = (HttpURLConnection) url.openConnection();
            connection.connect();

            if (connection.getResponseCode() != HttpURLConnection.HTTP_OK) {
                throw new ApiException(ErrorContainer.DOWNLOAD_FILES_ERROR);
            }

            long fileLength = connection.getContentLengthLong();
            input = connection.getInputStream();

            File file = new File(GAME_PATH + fileInfo.getPath().replace("files/", ""));

            if (file.exists()) {
                file.delete();
            }

            file.getParentFile().mkdirs();

            output = new FileOutputStream(GAME_PATH + fileInfo.getPath().replace("files/", ""));

            byte[] data = new byte[4096];
            int count;
            while ((count = input.read(data)) != -1) {
                if (isCancelledDownload()) {
                    input.close();
                    throw new ApiException(ErrorContainer.DOWNLOAD_WAS_INTERRUPTED);
                }
                total += count;

                if (fileLength > 0) {
                    publishProgress((int) (total * 100 / fileLengthFull));
                }

                fileLengthMin.set(total);

                output.write(data, 0, count);
            }
        } catch (ApiException e) {
            throw new ApiException(e);
        } catch (Exception e) {
            throw new ApiException(ErrorContainer.DOWNLOAD_FILES_ERROR);
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

    public boolean isCancelledDownload() {
        return isCancel;
    }

    public void cancelDownload() {
        this.isCancel = true;
    }

    private GameFileInfoDto getGameFilesInfo() {
        URI url = URI.create(FILE_INFO_URL);

        RestTemplate restTemplate = new RestTemplate();
        restTemplate.getMessageConverters().add(new MappingJackson2HttpMessageConverter());
        ResponseEntity<GameFileInfoDto> response = restTemplate.exchange(url, HttpMethod.GET, HttpEntity.EMPTY, GameFileInfoDto.class);

        return response.getBody();
    }

    private AsyncTaskResult<AsyncTask.Status> downloadLauncher(String uri) {
        fileLengthMin = new AtomicLong(0);
        total = 0;
        InputStream input = null;
        OutputStream output = null;
        HttpURLConnection connection = null;

        try {
            URL url = new URL(uri);
            connection = (HttpURLConnection) url.openConnection();
            connection.connect();

            if (connection.getResponseCode() != HttpURLConnection.HTTP_OK) {
                throw new ApiException(ErrorContainer.SERVER_CONNECT_ERROR);
            }

            int fileLength = connection.getContentLength();
            fileLengthFull = fileLength;
            input = connection.getInputStream();

            File file = new File(PATH_DOWNLOADS, "launcher.apk");

            if (file.exists()) {
                file.delete();
            }

            output = new FileOutputStream(PATH_DOWNLOADS + "launcher.apk");

            byte[] data = new byte[4096];

            int count;

            while ((count = input.read(data)) != -1) {
                if (isCancelledDownload()) {
                    input.close();
                    throw new ApiException(ErrorContainer.DOWNLOAD_WAS_INTERRUPTED);
                }
                total += count;

                if (fileLength > 0) {
                    publishProgress((int) (total * 100 / fileLength));
                }

                fileLengthMin.set(total);

                output.write(data, 0, count);
            }
        } catch (ApiException apiException) {
            return new AsyncTaskResult<>(apiException);
        } catch (Exception e) {
            return new AsyncTaskResult<>(new ApiException(ErrorContainer.DOWNLOAD_FILES_ERROR));
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

        return new AsyncTaskResult<>(AsyncTask.Status.FINISHED);
    }


    @Override
    protected void onPreExecute() {
        super.onPreExecute();
        PowerManager pm = (PowerManager) loaderActivity.getSystemService(Context.POWER_SERVICE);
        mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, getClass().getName());
        mWakeLock.acquire();
        progressBar.setVisibility(View.VISIBLE);
        progressBar.setIndeterminate(false);
        progressBar.setMax(100);
    }

    @Override
    protected void onProgressUpdate(Integer... progress) { //TODO попробовать исправить на Long
        super.onProgressUpdate(progress[0]);

        if (Utils.getType() == 0) {
            loaderActivity.getLoading().setText("Загрузка файлов игры...");
            loaderActivity.getLoadingPercent().setText(progress[0] + "%");
            loaderActivity.getFileName().setText(formatFileSize(fileLengthMin.longValue())+" из "+formatFileSize(fileLengthFull));
        }

        if (Utils.getType() == 1) {
            loaderActivity.getLoading().setText("Загрузка лаунчера...");
            loaderActivity.getLoadingPercent().setText(progress[0] + "%");
        }
        progressBar.setProgress(progress[0]);
    }

    @Override
    protected void onPostExecute(AsyncTaskResult<AsyncTask.Status> result) {
        mWakeLock.release();
        loaderActivity.getLoading().setVisibility(View.INVISIBLE);
        loaderActivity.getLoadingPercent().setVisibility(View.INVISIBLE);
        progressBar.setVisibility(View.INVISIBLE);

        if (result.getException() != null ) {
            ApiException apiException = result.getException();
            showErrorMessage(apiException);
            onAsyncErrorDo();
        } else if (isCancelled()) {
            showErrorMessage(new ApiException(ErrorContainer.DOWNLOAD_WAS_INTERRUPTED));
        } else if (onAsyncSuccessListener != null) {
            onAsyncSuccessListener.onSuccess();
        }
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
}
