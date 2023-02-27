package com.liverussia.launcher.ui.activity;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.core.content.FileProvider;
import androidx.viewpager2.widget.ViewPager2;

import com.google.firebase.crashlytics.FirebaseCrashlytics;
import com.liverussia.cr.BuildConfig;
import com.liverussia.cr.R;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;

import com.liverussia.cr.core.Samp;
import com.liverussia.launcher.domain.messages.InfoMessage;
import com.liverussia.launcher.utils.MainUtils;
import com.liverussia.cr.core.GTASA;
import com.liverussia.launcher.async.task.DownloadTask;
import com.liverussia.launcher.domain.LoaderSliderItemData;
import com.liverussia.launcher.async.dto.response.LoaderSliderInfoResponseDto;
import com.liverussia.launcher.domain.enums.DownloadType;
import com.liverussia.launcher.domain.enums.NativeStorageElements;
import com.liverussia.launcher.domain.messages.ErrorMessage;
import com.liverussia.launcher.async.service.NetworkService;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;
import com.liverussia.launcher.storage.NativeStorage;
import com.liverussia.launcher.ui.adapters.LoaderSliderAdapter;

import org.apache.commons.lang3.StringUtils;

import lombok.Getter;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

import static com.liverussia.launcher.config.Config.DOWNLOAD_DIRECTORY_NAME;
import static com.liverussia.launcher.config.Config.LIVE_RUSSIA_RESOURCE_SERVER_URL;
import static com.liverussia.launcher.config.Config.UPDATED_APK_PATH;

public class LoaderActivity extends Activity {

    private final static String IS_AFTER_LOADING_KEY = "isAfterLoading";
    private final static String APK_DATA_TYPE = "application/vnd.android.package-archive";
    private final static String FILE_PROVIDER_EXTENSION = ".provider";
    private final static int EXIT_SUCCESS_STATUS = 0;
    private final static int LAST_VERSION_WITHOUT_NEED_PERMS = 23;

    private final ActivityService activityService;

    private DownloadTask downloadTask;

    @Getter
    private TextView loading;
    @Getter
    private TextView repeatLoadButton;
    @Getter
    private TextView loadingPercent;
    @Getter
    private TextView fileName;
    private ProgressBar progressBar;
    private ImageView leftButton;
    private ImageView rightButton;
    private ViewPager2 sliderView;

    {
        activityService = new ActivityServiceImpl();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_loader);

        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);

        initialize();

        if (Build.VERSION.SDK_INT >= LAST_VERSION_WITHOUT_NEED_PERMS) {
            if (checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED
                    || checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED) {
                requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1000);
            } else {
                installGame(MainUtils.getType());
            }
        } else {
            installGame(MainUtils.getType());
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 1000) {
            installGame(MainUtils.getType());
        }
    }

    public static void writeFile(String path, String str) {
        File file = new File(path);

        try {
            if (!file.exists())
                file.createNewFile();
        } catch (IOException e) {
            e.printStackTrace();
        }
        FileWriter fileWriter = null;

        try {
            fileWriter = new FileWriter(new File(path), false);
            fileWriter.write(str);
            fileWriter.flush();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (fileWriter != null)
                    fileWriter.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private void writeLog(Exception e) {
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        e.printStackTrace(pw);
        String text = sw.toString();
        writeFile(getExternalFilesDir(null).toString()+"/log.txt", text);
    }

    private void redirectToSettings() {
        Intent intent = new Intent(this, com.liverussia.launcher.ui.activity.SplashActivity.class);
        intent.putExtras(getIntent());
        intent.putExtra(IS_AFTER_LOADING_KEY, true);
        startActivity(intent);
        finish();
    }

    private void redirectToMonitoring() {
        Intent intent = new Intent(this, com.liverussia.launcher.ui.activity.SplashActivity.class);
        intent.putExtras(getIntent());
        startActivity(intent);
        finish();
    }

    private void installGame(DownloadType type) {
        switch (type) {
            case LOAD_ALL_CACHE: {
                downloadTask = new DownloadTask(this, progressBar);
                downloadTask.setOnAsyncSuccessListener(this::performAfterLoadCacheSuccess);
                downloadTask.setOnAsyncCriticalErrorListener(this::performAfterDownloadFailed);
                downloadTask.download();
                break;
            }

            case RELOAD_OR_ADD_PART_OF_CACHE: {
                downloadTask = new DownloadTask(this, progressBar);
                downloadTask.setOnAsyncSuccessListener(this::performAfterReloadCacheSuccess);
                downloadTask.setOnAsyncCriticalErrorListener(this::performAfterDownloadFailed);
                downloadTask.reloadCache();
                break;
            }

            case UPDATE_APK: {
                downloadTask = new DownloadTask(this, progressBar);
                downloadTask.setOnAsyncSuccessListener(this::installApk);
                downloadTask.setOnAsyncCriticalErrorListener(this::performAfterUpdateApkFailed);
                downloadTask.updateApk();
            }
        }
    }

    private void performAfterDownloadFailed() {
        startActivity(new Intent(this, com.liverussia.launcher.ui.activity.SplashActivity.class));
    }

    private void performAfterLoadCacheSuccess() {
        redirectToSettings();
    }

    private void performAfterReloadCacheSuccess() {
        String nickname = NativeStorage.getClientProperty(NativeStorageElements.NICKNAME, this);
        String selectedServer = NativeStorage.getClientProperty(NativeStorageElements.SERVER, this);

        if (StringUtils.isBlank(nickname)) {
            activityService.showMessage(InfoMessage.DOWNLOAD_SUCCESS_INPUT_YOUR_NICKNAME.getText(), this);
            redirectToSettings();
            return;
        }

        if (StringUtils.isBlank(selectedServer)) {
            activityService.showMessage(InfoMessage.DOWNLOAD_SUCCESS_SELECT_SERVER.getText(), this);
            redirectToMonitoring();
            return;
        }

        startActivity(new Intent(this, Samp.class));

        this.finish();
    }

    private void performAfterUpdateApkFailed() {
        finish();
        System.exit(EXIT_SUCCESS_STATUS);
    }

    private void initialize() {
        Animation animation = AnimationUtils.loadAnimation(this, R.anim.button_click);
        loading = (TextView) findViewById(R.id.loadingText);
        loadingPercent = (TextView) findViewById(R.id.loadingPercent);
        progressBar = (ProgressBar) findViewById(R.id.progressBar);
        fileName = (TextView) findViewById(R.id.fileName);
        leftButton = (ImageView) findViewById(R.id.leftButton);
        rightButton = (ImageView) findViewById(R.id.rightButton);
        sliderView = findViewById(R.id.loaderSliderView);
        repeatLoadButton = (TextView) findViewById(R.id.repeatLoad);


        repeatLoadButton.setOnClickListener(v -> {
            v.startAnimation(animation);
            performRepeatLoadButtonAction();
        });

        leftButton.setOnClickListener(v -> {
            v.startAnimation(animation);
            performLeftButton();
        });

        rightButton.setOnClickListener(v -> {
            v.startAnimation(animation);
            performRightButton();
        });


        loadResources();

    }

    private void performRepeatLoadButtonAction() {
        downloadTask.repeatLoad();
    }

    private void loadResources() {
        Retrofit retrofit = new Retrofit.Builder()
                .baseUrl(LIVE_RUSSIA_RESOURCE_SERVER_URL)
                .addConverterFactory(GsonConverterFactory.create())
                .build();

        NetworkService networkService = retrofit.create(NetworkService.class);

        Call<LoaderSliderInfoResponseDto> call = networkService.getLoaderSliderInfo();

        call.enqueue(new Callback<LoaderSliderInfoResponseDto>() {
            @Override
            public void onResponse(Call<LoaderSliderInfoResponseDto> call, Response<LoaderSliderInfoResponseDto> response) {
                if (response.isSuccessful()) {
                    configureSlider(response.body());
                } else {
                    configureSliderWithoutData();
                }
            }

            @Override
            public void onFailure(Call<LoaderSliderInfoResponseDto> call, Throwable t) {
                configureSliderWithoutData();
            }
        });
    }

    private void configureSliderWithoutData() {
        List<LoaderSliderItemData> sliderDataArrayList = new ArrayList<>();

        LoaderSliderItemData sliderItemData = new LoaderSliderItemData();
        sliderItemData.setText(ErrorMessage.FAILED_LOAD_LOADER_SLIDER_DATA.getText());

        sliderDataArrayList.add(sliderItemData);

        initSlider(sliderDataArrayList);
    }

    private void configureSlider(LoaderSliderInfoResponseDto loaderSliderInfoResponseDto) {
        List<LoaderSliderItemData> sliderDataArrayList = Optional.ofNullable(loaderSliderInfoResponseDto.getTexts())
                .orElse(Collections.emptyList())
                .stream()
                .map(LoaderSliderItemData::new)
                .collect(Collectors.toList());

        initSlider(sliderDataArrayList);
    }

    private void initSlider(List<LoaderSliderItemData> sliderDataArrayList) {

        LoaderSliderAdapter adapter = new LoaderSliderAdapter(sliderDataArrayList);

        sliderView.setAdapter(adapter);

        sliderView.setOffscreenPageLimit(1);
        sliderView.setClipToPadding(false);
        sliderView.setClipChildren(false);

        sliderView.setCurrentItem(1, false);

        sliderView.registerOnPageChangeCallback(new ViewPager2.OnPageChangeCallback() {

            @Override
            public void onPageSelected(int position)
            {
                super.onPageSelected(position);
            }

            @Override
            public void onPageScrollStateChanged(int state)
            {
                if (state == ViewPager2.SCROLL_STATE_IDLE) {
                    if (sliderView.getCurrentItem() == sliderDataArrayList.size() + 2 - 1) {
                        sliderView.setCurrentItem(1, false);
                    } else if (sliderView.getCurrentItem() == 0) {
                        sliderView.setCurrentItem(sliderDataArrayList.size() + 2 - 2, false);
                    }
                }

                super.onPageScrollStateChanged(state);
            }
        });

    }

    private void performRightButton() {
        sliderView.setCurrentItem(sliderView.getCurrentItem() + 1);
    }

    private void performLeftButton() {
        sliderView.setCurrentItem(sliderView.getCurrentItem() - 1);
    }

    public void showMessage(String _s) {
        Toast.makeText(getApplicationContext(), _s, Toast.LENGTH_LONG).show();
    }

    @Override
    public void onBackPressed() {
        finish();
        System.exit(EXIT_SUCCESS_STATUS);
    }

    private void installApk() {
        try {
            File file = new File(getExternalFilesDir(null).toString() + DOWNLOAD_DIRECTORY_NAME,  UPDATED_APK_PATH);
            Intent intent;

            if (file.exists()) {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                    Uri apkUri = FileProvider.getUriForFile(this, BuildConfig.APPLICATION_ID + FILE_PROVIDER_EXTENSION, file);
//                    intent = new Intent(Intent.ACTION_INSTALL_PACKAGE);
                    intent = new Intent(Intent.ACTION_VIEW);
                    intent.setData(apkUri);
                    intent.setFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
                } else {
                    Uri apkUri = Uri.fromFile(file);
                    intent = new Intent(Intent.ACTION_VIEW);
                    intent.setDataAndType(apkUri, APK_DATA_TYPE);
                    intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                }
                startActivity(intent);
                finish();
            }
            else {
                activityService.showMessage(ErrorMessage.APK_UPDATE_FILE_NOT_FOUND.getText(), this);
                finish();
                System.exit(EXIT_SUCCESS_STATUS);
            }
        } catch (Exception e) {
            FirebaseCrashlytics.getInstance().recordException(e);
            finish();
            System.exit(EXIT_SUCCESS_STATUS);
        }
    }
}
