package com.liverussia.launcher.async;

import androidx.annotation.WorkerThread;

import com.liverussia.launcher.activity.MainActivity;
import com.liverussia.launcher.async.domain.AsyncTaskResult;
import com.liverussia.launcher.async.listener.OnAsyncCriticalErrorListener;
import com.liverussia.launcher.async.listener.OnAsyncSuccessListener;
import com.liverussia.launcher.dto.response.FileInfo;
import com.liverussia.launcher.dto.response.GameFileInfoDto;
import com.liverussia.launcher.error.apiException.ApiException;
import com.liverussia.launcher.error.apiException.ErrorContainer;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;
import com.techyourchance.threadposter.BackgroundThreadPoster;
import com.techyourchance.threadposter.UiThreadPoster;

import org.springframework.http.HttpEntity;
import org.springframework.http.HttpMethod;
import org.springframework.http.ResponseEntity;
import org.springframework.http.converter.json.MappingJackson2HttpMessageConverter;
import org.springframework.web.client.RestTemplate;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URI;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Collections;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;

import static com.liverussia.launcher.config.Config.FILE_INFO_URL;
import static com.liverussia.launcher.config.Config.GAME_PATH;

public class CacheChecker implements Listener<FileInfo[]> {

    private final MainActivity mainActivity;

    private final ActivityService activityService;
    private final UiThreadPoster uiThreadPoster;
    private final BackgroundThreadPoster backgroundThreadPoster;

    private OnAsyncSuccessListener onAsyncSuccessListener;
    private OnAsyncCriticalErrorListener onAsyncCriticalErrorListener;

    public CacheChecker(MainActivity mainActivity) {
        this.mainActivity = mainActivity;
    }

    {
        activityService = new ActivityServiceImpl();
        uiThreadPoster = new UiThreadPoster();
        backgroundThreadPoster = new BackgroundThreadPoster();
    }

    public void checkCache() {
        backgroundThreadPoster.post(() -> checkFilesHash());
    }

    @WorkerThread
    private void checkFilesHash() {
        try {
            GameFileInfoDto gameFilesInfo = getGameFilesInfo();
            checkFiles(gameFilesInfo);

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

    private void checkFiles(GameFileInfoDto gameFileInfo) {
        List<FileInfo> filesToReload = Optional.ofNullable(gameFileInfo.getFiles())
                .orElse(Collections.emptyList())
                .stream()
                .filter(this::isInvalidFile)
                .peek(this::removeFile)
                .collect(Collectors.toList());

        onAsyncFinished(new AsyncTaskResult<>(filesToReload.toArray(new FileInfo[0])));
    }

    private void removeFile(FileInfo fileInfo) {
        String filePath = fileInfo.getPath().replace("files/", "");
        File file = new File(GAME_PATH.concat(filePath));

        if (file.exists()) {
            file.delete();
        }
    }

    private boolean isInvalidFile(FileInfo fileInfo) {
        String filePath = fileInfo.getPath().replace("files/", "");
        File file = new File(GAME_PATH.concat(filePath));

        return !file.exists() || !isValidHash(filePath, fileInfo);
    }

    private boolean isValidHash(String filePath, FileInfo fileInfo) {
        String hash = calculateHash(GAME_PATH.concat(filePath));

        return hash.equals(fileInfo.getHash());
    }

    public String calculateHash(String filename) {
        try {
            MessageDigest md = MessageDigest.getInstance("MD5");
            byte[] buffer = new byte[8192];
            try (InputStream is = new FileInputStream(filename)) {
                int read;
                while ((read = is.read(buffer)) > 0) {
                    md.update(buffer, 0, read);
                }
            }
            byte[] digest = md.digest();
            return bytesToHex(digest);
        } catch (NoSuchAlgorithmException | IOException e) {
            throw new RuntimeException(e);
        }
    }

    private GameFileInfoDto getGameFilesInfo() {
        URI url = URI.create(FILE_INFO_URL);

        RestTemplate restTemplate = new RestTemplate();
        restTemplate.getMessageConverters().add(new MappingJackson2HttpMessageConverter());
        ResponseEntity<GameFileInfoDto> response = restTemplate.exchange(url, HttpMethod.GET, HttpEntity.EMPTY, GameFileInfoDto.class);

        return response.getBody();
    }

    private String bytesToHex(byte[] raw) {
        if (raw == null) {
            return null;
        } else {
            StringBuilder hex = new StringBuilder(2 * raw.length);
            byte[] var2 = raw;
            int var3 = raw.length;

            for(int var4 = 0; var4 < var3; ++var4) {
                byte b = var2[var4];
                hex.append(Character.forDigit((b & 240) >> 4, 16)).append(Character.forDigit(b & 15, 16));
            }

            return hex.toString();
        }
    }

    public void setOnAsyncSuccessListener(OnAsyncSuccessListener onClickListener) {
        this.onAsyncSuccessListener = onClickListener;
    }

    public void setOnAsyncCriticalErrorListener(OnAsyncCriticalErrorListener onAsyncCriticalErrorListener) {
        this.onAsyncCriticalErrorListener = onAsyncCriticalErrorListener;
    }

    @Override
    public void onAsyncFinished(AsyncTaskResult<FileInfo[]> result) {
        if (result.getException() != null) {
            ApiException apiException = result.getException();

            //TODO вернуть как было
            activityService.showBigMessage(apiException.getMessage(), mainActivity);
//            showErrorMessage(apiException);
            onAsyncErrorDo();
//        } else if (isCancelled()) {
//            showErrorMessage(new ApiException(ErrorContainer.DOWNLOAD_WAS_INTERRUPTED));
        } else if (onAsyncSuccessListener != null) {
            onAsyncSuccessListener.onSuccess();
        }
    }

    private void onAsyncErrorDo() {
        if (onAsyncCriticalErrorListener != null) {
            onAsyncCriticalErrorListener.onCriticalError();
        }
    }
}
