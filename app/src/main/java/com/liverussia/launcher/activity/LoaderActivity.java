package com.liverussia.launcher.activity;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.FileProvider;
import androidx.viewpager2.widget.ViewPager2;

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

import com.liverussia.cr.core.DownloadUtils;
import com.liverussia.launcher.async.DownloadTask;
import com.liverussia.launcher.domain.LoaderSliderItemData;
import com.liverussia.launcher.dto.response.LoaderSliderInfoResponseDto;
import com.liverussia.launcher.enums.DownloadType;
import com.liverussia.launcher.messages.ErrorMessages;
import com.liverussia.launcher.other.NetworkService;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;
import com.liverussia.launcher.ui.adapters.LoaderSliderAdapter;

import lombok.Getter;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

import static com.liverussia.launcher.config.Config.LAUNCHER_SERVER_URI;

public class LoaderActivity extends AppCompatActivity {

    private final static String APK_NAME = "launcher";
    private final static String IS_AFTER_LOADING_KEY = "isAfterLoading";

    private final ActivityService activityService;

    private DownloadTask downloadTask;

    @Getter
    private TextView loading;
    @Getter
    private TextView repeatLoadButton;

    @Getter
    private TextView loadingPercent;
    private ProgressBar progressBar;

    @Getter
    private TextView fileName;
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

        if (Build.VERSION.SDK_INT >= 23) {
            if (checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED
                    || checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED) {
                requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1000);
            } else {
                installGame(DownloadUtils.getType());
            }
        } else {
            installGame(DownloadUtils.getType());
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 1000) {
            installGame(DownloadUtils.getType());
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
        Intent intent = new Intent(this, com.liverussia.launcher.activity.SplashActivity.class);
        intent.putExtras(getIntent());
        intent.putExtra(IS_AFTER_LOADING_KEY, true);
        startActivity(intent);
        finish();
    }

//    @Override
//    public void onStop() {
//        Optional.ofNullable(downloadTask)
//                .ifPresent(DownloadAsyncTask1::cancelDownload);
//
//        super.onStop();
//    }
//
//    @Override
//    public void onDestroy() {
//        Optional.ofNullable(downloadTask)
//                .ifPresent(DownloadAsyncTask1::cancelDownload);
//
//        super.onDestroy();
//    }

    private void installGame(DownloadType type) {
        switch (type) {
            case LOAD_ALL_CACHE: {
//                File dir = new File(getExternalFilesDir(null).toString() + "/temp_downloads/");
//
//                if (!dir.exists()) {
//                    dir.mkdirs();
//                }

                downloadTask = new DownloadTask(this, progressBar);
                downloadTask.setOnAsyncSuccessListener(this::performAfterDownload);
                downloadTask.setOnAsyncCriticalErrorListener(this::performAfterDownloadFailed);
                downloadTask.download();
                break;
            }

            case RELOAD_OR_ADD_PART_OF_CACHE: {
                downloadTask = new DownloadTask(this, progressBar);
                downloadTask.setOnAsyncSuccessListener(this::performAfterDownload);
                downloadTask.setOnAsyncCriticalErrorListener(this::performAfterDownloadFailed);
                downloadTask.reloadCache();
                break;
            }

//            case 1: {
//                File dir = new File(getExternalFilesDir(null).toString() + "/temp_downloads/");
//                if (!dir.exists()) dir.mkdirs();
//                DownloadAsyncTask1 downloadTask = new DownloadAsyncTask1(this, progressBar);
//                downloadTask.execute(URL_CLIENT);
//                break;
//            }
        }
    }

    private void performAfterDownloadFailed() {
        startActivity(new Intent(this, com.liverussia.launcher.activity.SplashActivity.class));
    }

    private void performAfterDownload() {
        if (DownloadType.LOAD_ALL_CACHE.equals(DownloadUtils.getType())) {
            redirectToSettings();
        }

//        if (Utils.getType() == 1) {
//            showMessage(InfoMessages.APPROVE_INSTALL.getText());
//            installAPK(APK_NAME);
//        }
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
                .baseUrl(LAUNCHER_SERVER_URI)
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
        sliderItemData.setText(ErrorMessages.FAILED_LOAD_LOADER_SLIDER_DATA.getText());

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

    private void installAPK(final String apkname) {
        try {
            File file = new File(getExternalFilesDir(null).toString() + "/temp_downloads/", apkname + ".apk");
            Intent intent;
            if (file.exists()) {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                    Uri apkUri = FileProvider.getUriForFile(this, BuildConfig.APPLICATION_ID + ".provider", file);
                    intent = new Intent(Intent.ACTION_INSTALL_PACKAGE);
                    intent.setData(apkUri);
                    intent.setFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
                } else {
                    Uri apkUri = Uri.fromFile(file);
                    intent = new Intent(Intent.ACTION_VIEW);
                    intent.setDataAndType(apkUri, "application/vnd.android.package-archive");
                    intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                }
                startActivity(intent);
                finish();
            }
            else showMessage("Ошибка установки: файл не найден");
        } catch (Exception e) {
            Log.e("InstallAPK", "Ошибка установки:" + e.getMessage());
            writeLog(e);
        }
    }
}
