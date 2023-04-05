package com.liverussia.cr;

import static com.liverussia.launcher.config.Config.DOWNLOAD_DIRECTORY_NAME;
import static com.liverussia.launcher.config.Config.LIVE_RUSSIA_RESOURCE_SERVER_URL;
import static com.liverussia.launcher.config.Config.UPDATED_APK_PATH;

import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.FileProvider;

import com.google.android.gms.common.util.CollectionUtils;
import com.google.android.material.progressindicator.LinearProgressIndicator;
import com.google.firebase.crashlytics.FirebaseCrashlytics;
import com.liverussia.cr.core.Samp;
import com.liverussia.launcher.async.dto.response.FileInfo;
import com.liverussia.launcher.async.dto.response.LatestVersionInfoDto;
import com.liverussia.launcher.async.service.NetworkService;
import com.liverussia.launcher.async.task.CacheChecker;
import com.liverussia.launcher.async.task.DownloadTask;
import com.liverussia.launcher.domain.enums.DownloadType;
import com.liverussia.launcher.domain.messages.ErrorMessage;
import com.liverussia.launcher.utils.MainUtils;

import java.io.File;
import java.util.Arrays;
import java.util.List;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

public class StartActivity extends AppCompatActivity {
    NetworkService networkService;

    LinearProgressIndicator progressIndicator;
    TextView progressText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);

        setContentView(R.layout.activity_cachechecker);

        super.onCreate(savedInstanceState);

        progressIndicator = findViewById(R.id.progressIndicator);
        progressText = findViewById(R.id.loading_text);

        Retrofit retrofit = new Retrofit.Builder()
                .baseUrl(LIVE_RUSSIA_RESOURCE_SERVER_URL)
                .addConverterFactory(GsonConverterFactory.create())
                .build();

        networkService = retrofit.create(NetworkService.class);
        loadLastApkInfo();
    }

    void updateLoadingText(String text){
        TextView loading_text = findViewById(R.id.loading_text);
        loading_text.setText(text);
    }

    // ------------- Check cache
    void checkCache()
    {
        CacheChecker cacheChecker = new CacheChecker(this);
        cacheChecker.setOnAsyncSuccessListener(this::downloadCache);
        cacheChecker.validateCache();
    }

    void downloadCache(FileInfo[] fileToReloadArray) {

        List<FileInfo> filesToReloadList = Arrays.asList(fileToReloadArray);

        if (CollectionUtils.isEmpty(filesToReloadList)) {
            onDownloadedCache();
        }
        else {
            MainUtils.FILES_TO_RELOAD = filesToReloadList;
            MainUtils.setType(DownloadType.RELOAD_OR_ADD_PART_OF_CACHE);

            DownloadTask downloadTask = new DownloadTask(this, progressIndicator, progressText);
            downloadTask.setOnAsyncSuccessListener(this::onDownloadedCache);
          //  downloadTask.setOnAsyncCriticalErrorListener(this::performAfterDownloadFailed);
            downloadTask.reloadCache();
        }
    }

    void onDownloadedCache() {
        startActivity(new Intent(this, Samp.class));
        this.finish();
    }

    // ------------- Check apk
    int getCurrApkVersion(){
        PackageManager pm = this.getPackageManager();

        try {
            PackageInfo pInfo = pm.getPackageInfo(this.getPackageName(),0);
            return pInfo.versionCode;
        } catch (PackageManager.NameNotFoundException e1) {
            e1.printStackTrace();
        }

        finishAffinity();

        return 0;
    }

    void loadLastApkInfo()
    {
        Call<LatestVersionInfoDto> apkInfo = networkService.getLastApkInfo();
        apkInfo.enqueue(new Callback<LatestVersionInfoDto>() {
            @Override
            public void onResponse(Call<LatestVersionInfoDto> call, Response<LatestVersionInfoDto> response) {
                if (!response.isSuccessful()) {
                    finishAffinity();
                    return;
                }
                checkApkVersion(response.body());
            }

            @Override
            public void onFailure(Call<LatestVersionInfoDto> call, Throwable t) {
                finishAffinity();
            }
        });
    }

    void checkApkVersion(LatestVersionInfoDto apkInfo)
    {
        int lastVer = Integer.parseInt(apkInfo.version);

        if(lastVer > getCurrApkVersion())
        {
            MainUtils.setType(DownloadType.UPDATE_APK);
            MainUtils.LATEST_APK_INFO = apkInfo;

            progressIndicator.setIndeterminate(false);
            DownloadTask downloadTask = new DownloadTask(this, progressIndicator, progressText);
            downloadTask.setOnAsyncSuccessListener(this::installApk);
        //    downloadTask.setOnAsyncCriticalErrorListener(this::performAfterUpdateApkFailed);
            downloadTask.updateApk();
            return;
        }
        progressIndicator.setIndeterminate(true);
        progressText.setText("Проверка файлов игры ...");
        checkCache();
    }
    void installApk() {
        try {
            File file = new File(getExternalFilesDir(null).toString() + DOWNLOAD_DIRECTORY_NAME + UPDATED_APK_PATH);

            if (file.exists()) {
                Intent intent = new Intent(Intent.ACTION_VIEW);
                Uri uri = FileProvider.getUriForFile(getApplicationContext(), BuildConfig.APPLICATION_ID + ".provider", file);
                intent.setDataAndType(uri, "application/vnd.android.package-archive");
                intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
                try {
                    getApplicationContext().startActivity(intent);
                } catch (ActivityNotFoundException e) {
                    e.printStackTrace();
                    Log.e("TAG", "Error in opening the file!");
                }
            }
            else {
                Toast.makeText(this, "Ошибка установки: файл не найден", Toast.LENGTH_SHORT).show();
                finishAffinity();
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
            finishAffinity();
        }
    }
}