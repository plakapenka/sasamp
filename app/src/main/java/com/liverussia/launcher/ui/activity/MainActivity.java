package com.liverussia.launcher.ui.activity;

import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Build;

import android.os.Handler;
import android.util.Log;
import android.widget.*;
import android.graphics.PorterDuff;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import android.content.Intent;

import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentTransaction;

import android.view.View;
import android.view.animation.AnimationUtils;
import android.view.animation.Animation;

import com.liverussia.cr.R;
import com.liverussia.launcher.utils.MainUtils;
import com.liverussia.cr.core.GTASA;
import com.liverussia.launcher.async.task.CacheChecker;
import com.liverussia.launcher.async.dto.response.FileInfo;
import com.liverussia.cr.gui.MineGame3;
import com.liverussia.launcher.async.dto.response.ServerImagesResponseDto;
import com.liverussia.launcher.domain.enums.DownloadType;
import com.liverussia.launcher.domain.enums.NativeStorageElements;
import com.liverussia.launcher.ui.fragment.MonitoringFragment;
import com.liverussia.launcher.ui.fragment.DonateFragment;
import com.liverussia.launcher.ui.fragment.RouletteFragment;
import com.liverussia.launcher.ui.fragment.SettingsFragment;
import com.liverussia.launcher.domain.messages.ErrorMessage;
import com.liverussia.launcher.async.dto.response.Servers;
import com.liverussia.launcher.async.service.NetworkService;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;
import com.liverussia.launcher.storage.NativeStorage;

import org.apache.commons.lang3.StringUtils;
import org.springframework.util.CollectionUtils;

import java.io.File;

import java.util.*;

import lombok.Getter;
import lombok.Setter;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

import static com.liverussia.launcher.config.Config.LIVE_RUSSIA_RESOURCE_SERVER_URL;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    private final static String IS_AFTER_LOADING_KEY = "isAfterLoading";
    private final static int GAME_DIRECTORY_EMPTY_SIZE = 0;
    private final static int LAST_VERSION_WITHOUT_NEED_PERMS = 23;

    private Handler handler;
    private Animation animation;
	public LinearLayout donateButton;
    public ImageView donateImage;
    public TextView donateTV;
    public LinearLayout monitoringButton;
    public MonitoringFragment monitoringFragment;
    public ImageView monitoringImage;
    public TextView monitoringTV;
    public LinearLayout playButton;
    public ImageView playImage;
    public LinearLayout rouletteButton;
    public ImageView rouletteImage;
    public TextView rouletteTV;
    public LinearLayout settingsButton;
    public SettingsFragment settingsFragment;
    private RouletteFragment rouletteFragment;
    private DonateFragment donateFragment;
    public ImageView settingsImage;
    public TextView settingsTV;
    public FrameLayout container_layout;

    private final ActivityService activityService;

    private ServerImagesResponseDto possiblePrizesInfoResponseDto;
    private ServerImagesResponseDto donateServicesResponseDto;

    public MineGame3 gg;

    @Getter
    @Setter
    private List<Servers> servers;

    {
        activityService = new ActivityServiceImpl();
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
        setTheme(R.style.AppBaseTheme);

//        setContentView(R.layout.mine_3);
//        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
//        gg = new MineGame3(this);
//
//        gg.Show();

        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE
                     //   | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);

        container_layout = findViewById(R.id.container);
        animation = AnimationUtils.loadAnimation(this, R.anim.button_click);

		monitoringTV = (TextView) findViewById(R.id.monitoringTV);
        settingsTV = (TextView) findViewById(R.id.settingsTV);
        rouletteTV = (TextView) findViewById(R.id.forumTV);
        donateTV = (TextView) findViewById(R.id.donateTV);

        monitoringImage = (ImageView) findViewById(R.id.monitoringImage);
        settingsImage = (ImageView) findViewById(R.id.settingsImage);
        rouletteImage = (ImageView) findViewById(R.id.forumImage);
        donateImage = (ImageView) findViewById(R.id.donateImage);
        playImage = (ImageView) findViewById(R.id.playImage);

        monitoringButton = (LinearLayout) findViewById(R.id.monitoringButton);
        settingsButton = (LinearLayout) findViewById(R.id.settingsButton);
        rouletteButton = (LinearLayout) findViewById(R.id.rouletteButton);
        donateButton = (LinearLayout) findViewById(R.id.donateButton);
        playButton = (LinearLayout) findViewById(R.id.playButton);

        this.handler = new Handler();
		monitoringFragment = new MonitoringFragment();
        settingsFragment = new SettingsFragment();

        if (savedInstanceState != null && savedInstanceState.getBoolean(IS_AFTER_LOADING_KEY)) {
            replaceFragment(settingsFragment);
        } else if (savedInstanceState == null && getIntent().getExtras() != null && getIntent().getExtras().getBoolean(IS_AFTER_LOADING_KEY)){
            onClickSettings();
        } else {
            replaceFragment(monitoringFragment);
        }

		monitoringButton.setOnClickListener(this);
        settingsButton.setOnClickListener(this);
        rouletteButton.setOnClickListener(this);
        donateButton.setOnClickListener(this);
        playButton.setOnClickListener(this);

        loadResources();
    }

    private void loadResources() {
        getPossiblePrizes();
        getDonateServices();
    }


    //TODO сделать это как в сплеше с монтрингом и выпилить отсюда
    private void getPossiblePrizes() {
        Retrofit retrofit = new Retrofit.Builder()
                .baseUrl(LIVE_RUSSIA_RESOURCE_SERVER_URL)
                .addConverterFactory(GsonConverterFactory.create())
                .build();

        NetworkService networkService = retrofit.create(NetworkService.class);

        Call<ServerImagesResponseDto> call = networkService.getPossibleRoulettePrizes();

        call.enqueue(new Callback<ServerImagesResponseDto>() {
            @Override
            public void onResponse(Call<ServerImagesResponseDto> call, Response<ServerImagesResponseDto> response) {
                if (response.isSuccessful()) {
                    addPossiblePrizes(response.body());
                }
            }

            @Override
            public void onFailure(Call<ServerImagesResponseDto> call, Throwable t) {
            }
        });
    }

    private void getDonateServices() {
        Retrofit retrofit = new Retrofit.Builder()
                .baseUrl(LIVE_RUSSIA_RESOURCE_SERVER_URL)
                .addConverterFactory(GsonConverterFactory.create())
                .build();

        NetworkService networkService = retrofit.create(NetworkService.class);

        Call<ServerImagesResponseDto> call = networkService.getDonateServices();

        call.enqueue(new Callback<ServerImagesResponseDto>() {
            @Override
            public void onResponse(Call<ServerImagesResponseDto> call, Response<ServerImagesResponseDto> response) {
                if (response.isSuccessful()) {
                    addDonateServices(response.body());
                }
            }

            @Override
            public void onFailure(Call<ServerImagesResponseDto> call, Throwable t) {
            }
        });
    }

    public void addDonateServices(ServerImagesResponseDto result) {
        if (donateFragment != null) {
            donateFragment.addDonateServices(result);
        }
        this.donateServicesResponseDto = result;
    }

    public void addPossiblePrizes(ServerImagesResponseDto result) {
        if (rouletteFragment != null) {
            rouletteFragment.addPossiblePrizes(result);
        }
        this.possiblePrizesInfoResponseDto = result;
    }

    public ServerImagesResponseDto getPossiblePrizesInfoResponseDto() {
        return this.possiblePrizesInfoResponseDto;
    }

    public ServerImagesResponseDto getDonateServicesResponseDto() {
        return this.donateServicesResponseDto;
    }

    @Override
    public void onClick(View v) {

        //todo сделать как в прогресс баре с загнрузкой а это выпилить
        if (isFragmentBlockProcessRunning()) {
            return;
        }

        switch (v.getId()) {
            case R.id.monitoringButton:
                v.startAnimation(animation);
                onClickMonitoring();
                break;
            case R.id.settingsButton:
                v.startAnimation(animation);
                onClickSettings();
                break;
            case R.id.rouletteButton:
                v.startAnimation(animation);
                onClickRoulette();
                break;
            case R.id.donateButton:
                v.startAnimation(animation);
                onClickDonate();
                break;
            case R.id.playButton:
                v.startAnimation(animation);
                this.handler.postDelayed(new Runnable() {
                    public final void run() {
                        onClickPlay();
                    }
                }, 200);

                break;
            default:
                break;
        }
    }

    private boolean isFragmentBlockProcessRunning() {
        return rouletteFragment != null && rouletteFragment.isRouletteSpinning();
    }

    public void onClickPlay() {
        String isTestMode = NativeStorage.getClientProperty(NativeStorageElements.TEST, this);

        if (StringUtils.isNotBlank(isTestMode) && Integer.parseInt(isTestMode) == 1) startGame();
        else {
      //  startGame();
            File gameDirectory = new File(this.getExternalFilesDir(null).toString());

            if (gameDirectory.list() != null && gameDirectory.list().length > GAME_DIRECTORY_EMPTY_SIZE) {
                CacheChecker cacheChecker = new CacheChecker(this);
                cacheChecker.setOnAsyncSuccessListener(this::doAfterCacheChecked);
                //TODO вернуть перед релизом
                //  cacheChecker.checkIsAllCacheFilesExist();
                cacheChecker.validateCache();
            } else {
                MainUtils.setType(DownloadType.LOAD_ALL_CACHE);
                startActivity(new Intent(this, LoaderActivity.class));
            }
        }
    }

    private void doAfterCacheChecked(FileInfo[] fileToReloadArray) {

        List<FileInfo> filesToReloadList = Arrays.asList(fileToReloadArray);

        if (CollectionUtils.isEmpty(filesToReloadList)) {
            startGame();
        } else {
            reloadCache(filesToReloadList);
        }
    }

    private void reloadCache(List<FileInfo> filesToReloadList) {
        MainUtils.FILES_TO_RELOAD = filesToReloadList;
        MainUtils.setType(DownloadType.RELOAD_OR_ADD_PART_OF_CACHE);
        startActivity(new Intent(this, LoaderActivity.class));
    }

    private void startGame() {
        String nickname = NativeStorage.getClientProperty(NativeStorageElements.NICKNAME, this);
        String selectedServer = NativeStorage.getClientProperty(NativeStorageElements.SERVER, this);

        if (StringUtils.isBlank(nickname)) {
            activityService.showMessage(ErrorMessage.INPUT_NICKNAME_BEFORE_SERVER_CONNECT.getText(), this);
            onClickSettings();
            return;
        }

        if (StringUtils.isBlank(selectedServer)) {
            activityService.showMessage(ErrorMessage.SERVER_NOT_SELECTED.getText(), this);
            onClickMonitoring();
            return;
        }
        startActivity(new Intent(this, GTASA.class));

        this.finish();
    }

    private void onClickRoulette() {
        setTextColor(rouletteButton, rouletteTV, rouletteImage);
        rouletteFragment = new RouletteFragment(this);
        replaceFragment(rouletteFragment);
    }

    public void onClickSettings() {
            setTextColor(settingsButton, settingsTV, settingsImage);
            replaceFragment(this.settingsFragment);
    }

    public void onClickDonate() {
            setTextColor(donateButton, donateTV, donateImage);
            donateFragment = new DonateFragment(this);
            replaceFragment(donateFragment);
    }
    
	public void onClickMonitoring() {
            setTextColor(monitoringButton, monitoringTV, monitoringImage);
            replaceFragment(monitoringFragment);
    }
	
    public void setTextColor(LinearLayout linearLayout, TextView textView, ImageView imageView) {
        monitoringButton.setAlpha(0.45f);
        settingsButton.setAlpha(0.45f);
        rouletteButton.setAlpha(0.45f);
        donateButton.setAlpha(0.45f);
        monitoringTV.setTextColor(getResources().getColor(R.color.menuTextDisable));
        settingsTV.setTextColor(getResources().getColor(R.color.menuTextDisable));
        rouletteTV.setTextColor(getResources().getColor(R.color.menuTextDisable));
        donateTV.setTextColor(getResources().getColor(R.color.menuTextDisable));
        monitoringImage.setColorFilter(getResources().getColor(R.color.menuTextDisable), PorterDuff.Mode.SRC_IN);
        settingsImage.setColorFilter(getResources().getColor(R.color.menuTextDisable), PorterDuff.Mode.SRC_IN);
        rouletteImage.setColorFilter(getResources().getColor(R.color.menuTextDisable), PorterDuff.Mode.SRC_IN);
        donateImage.setColorFilter(getResources().getColor(R.color.menuTextDisable), PorterDuff.Mode.SRC_IN);
        linearLayout.setAlpha(1.0f);
        textView.setTextColor(getResources().getColor(R.color.menuTextEnable));
        imageView.setColorFilter(getResources().getColor(R.color.menuTextEnable), PorterDuff.Mode.SRC_IN);
    }

    public void replaceFragment(Fragment fragment) {
        FragmentTransaction beginTransaction = getSupportFragmentManager().beginTransaction();
        beginTransaction.replace(R.id.container, fragment);
        beginTransaction.commit();
    }
	
	public boolean isRecordAudioPermissionGranted() {
        if (Build.VERSION.SDK_INT < LAST_VERSION_WITHOUT_NEED_PERMS || checkSelfPermission("android.permission.RECORD_AUDIO") == PackageManager.PERMISSION_GRANTED) {
            return true;
        }
        ActivityCompat.requestPermissions(this, new String[]{"android.permission.RECORD_AUDIO"}, 2);
        return false;
    }

    public boolean isStoragePermissionGranted() {
        if (Build.VERSION.SDK_INT < LAST_VERSION_WITHOUT_NEED_PERMS || checkSelfPermission("android.permission.WRITE_EXTERNAL_STORAGE") == 0) {
            return true;
        }
        ActivityCompat.requestPermissions(this, new String[]{"android.permission.WRITE_EXTERNAL_STORAGE"}, 1);
        return false;
    }

    @Override
	public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onRestart() {
        super.onRestart();
    }

    @Override
    public void onStop() {
        super.onStop();
    }
}