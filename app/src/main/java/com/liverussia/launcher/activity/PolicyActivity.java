package com.liverussia.launcher.activity;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.graphics.PorterDuff;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.core.app.ActivityCompat;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentTransaction;

import com.liverussia.cr.R;
import com.liverussia.cr.core.Config;
import com.liverussia.cr.core.DownloadUtils;
import com.liverussia.launcher.dto.response.LatestVersionInfoDto;
import com.liverussia.launcher.enums.DownloadType;
import com.liverussia.launcher.fragment.DonateFragment;
import com.liverussia.launcher.fragment.MonitoringFragment;
import com.liverussia.launcher.fragment.SettingsFragment;
import com.liverussia.launcher.other.NetworkService;

import java.io.File;
import java.util.Timer;
import java.util.TimerTask;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

import static com.liverussia.launcher.config.Config.LIVE_RUSSIA_RESOURCE_SERVER_URI;

public class PolicyActivity extends AppCompatActivity {

    public ConstraintLayout policy_cancel;
    public ConstraintLayout policy_ok;
    private NetworkService sNetworkService;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.policy_activity);

        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);

        Retrofit retrofit = new Retrofit.Builder()
                .baseUrl(LIVE_RUSSIA_RESOURCE_SERVER_URI)
                .addConverterFactory(GsonConverterFactory.create())
                .build();

        sNetworkService = retrofit.create(NetworkService.class);

		Animation animation = AnimationUtils.loadAnimation(this, R.anim.button_click);

        policy_cancel = (ConstraintLayout) findViewById(R.id.policy_cancel);
        policy_ok = (ConstraintLayout) findViewById(R.id.policy_ok);

        policy_cancel.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                v.startAnimation(animation);
                onClickCancel();
            }
        });

        policy_ok.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                v.startAnimation(animation);
                onClickOkey();
            }
        });
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 1000) {
            checkVersionAndStartLauncher();
        }
    }

    public void onClickOkey() {
        if (Build.VERSION.SDK_INT >= 23) {
            if (checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED
                    || checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED || checkSelfPermission(Manifest.permission.RECORD_AUDIO) == PackageManager.PERMISSION_DENIED) {
                requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.RECORD_AUDIO}, 1000);
            } else {
                checkVersionAndStartLauncher();
            }
        } else checkVersionAndStartLauncher();
    }

	public void onClickCancel() {
        System.exit(0);
    }

    private void startLauncher() {
        startActivity(new Intent(this, MainActivity.class));
        finish();
    }


    private void checkVersionAndStartLauncher() {
        Call<LatestVersionInfoDto> latestVersionInfoCall = sNetworkService.getLatestVersionInfoDto();

        latestVersionInfoCall.enqueue(new Callback<LatestVersionInfoDto>() {
            @Override
            public void onResponse(Call<LatestVersionInfoDto> call, Response<LatestVersionInfoDto> response) {
                if (!response.isSuccessful()) {
                    finish();
                    System.exit(0);
                }

                checkVersion(response.body());

            }

            @Override
            public void onFailure(Call<LatestVersionInfoDto> call, Throwable t) {
                finish();
                System.exit(0);
            }
        });
    }

    private void checkVersion(LatestVersionInfoDto latestVersionInfo) {
        String currentVersion = getCurrentVersion();
        String latestVersion = latestVersionInfo.getVersion();

        if (currentVersion.equals(latestVersion)) {
            startLauncher();
            return;
        }

        DownloadUtils.setType(DownloadType.UPDATE_APK);
        DownloadUtils.LATEST_APK_INFO = latestVersionInfo;
        startActivity(new Intent(this, LoaderActivity.class));
    }

    private String getCurrentVersion(){
        PackageManager pm = this.getPackageManager();
        PackageInfo pInfo = null;
        try {
            pInfo = pm.getPackageInfo(this.getPackageName(),0);
        } catch (PackageManager.NameNotFoundException e1) {
            e1.printStackTrace();
        }
        return pInfo.versionName;
    }

	public void onDestroy() {
        super.onDestroy();
    }

    public void onRestart() {
        super.onRestart();
    }
} 