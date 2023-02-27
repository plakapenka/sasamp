package com.liverussia.launcher.ui.activity;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.view.View;

import com.liverussia.cr.R;

import java.util.ArrayList;

import com.liverussia.launcher.utils.MainUtils;
import com.liverussia.launcher.async.dto.response.LatestVersionInfoDto;
import com.liverussia.launcher.async.dto.response.MonitoringData;
import com.liverussia.launcher.domain.enums.DownloadType;
import com.liverussia.launcher.error.apiException.ErrorContainer;

import com.liverussia.launcher.async.dto.response.Servers;
import com.liverussia.launcher.async.service.NetworkService;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

import static com.liverussia.launcher.config.Config.LIVE_RUSSIA_RESOURCE_SERVER_URL;

public class SplashActivity extends Activity {

	private final static String IS_AFTER_LOADING_KEY = "isAfterLoading";
	private final static int EXIT_SUCCESS_STATUS = 0;
	private final static int LAST_VERSION_WITHOUT_NEED_PERMS = 23;

	public static ArrayList<Servers> slist;

	private final NetworkService networkService;
	private final ActivityService activityService;

	{
		Retrofit retrofit = new Retrofit.Builder()
				.baseUrl(LIVE_RUSSIA_RESOURCE_SERVER_URL)
				.addConverterFactory(GsonConverterFactory.create())
				.build();

		networkService = retrofit.create(NetworkService.class);

		activityService = new ActivityServiceImpl();

		MainUtils.SERVERS = new ArrayList<>();
		MainUtils.NEWS = new ArrayList<>();
	}
	@Override
    protected void onCreate(Bundle savedInstanceState) {
		checkAndroidVersion();

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash);

		getWindow().getDecorView().setSystemUiVisibility(
				View.SYSTEM_UI_FLAG_IMMERSIVE
						| View.SYSTEM_UI_FLAG_LAYOUT_STABLE
						| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
						| View.SYSTEM_UI_FLAG_FULLSCREEN
						| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);

		loadMonitoringData();
	}

	private void checkAndroidVersion() {
		if (android.os.Build.VERSION.SDK_INT < Build.VERSION_CODES.N) {
			System.exit(EXIT_SUCCESS_STATUS);
		}
	}

	private void loadMonitoringData() {
		Call<MonitoringData> newsCall = networkService.getMonitoringData();

		newsCall.enqueue(new Callback<MonitoringData>() {
			@Override
			public void onResponse(Call<MonitoringData> call, Response<MonitoringData> response) {
				if (response.isSuccessful()) {
					MonitoringData monitoringData = response.body();

					saveData(monitoringData);
					startApp();
				}
			}

			@Override
			public void onFailure(Call<MonitoringData> call, Throwable t) {
				activityService.showMessage(ErrorContainer.SERVER_CONNECT_ERROR.getMessage(), SplashActivity.this);
				startApp();
			}
		});
	}

	private void saveData(MonitoringData monitoringData) {
		MainUtils.NEWS = monitoringData.getNews();
		MainUtils.SERVERS = monitoringData.getServers();
	}

	private void startApp() {
		if (Build.VERSION.SDK_INT >= LAST_VERSION_WITHOUT_NEED_PERMS) {
			if (checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED
					|| checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED ) {
				startActivity(new Intent(this, PolicyActivity.class));
				finish();
			} else {
				checkVersionAndStartLauncher();
			}
		} else {
			checkVersionAndStartLauncher();
		}
	}
	
	private void checkVersionAndStartLauncher() {
		Call<LatestVersionInfoDto> latestVersionInfoCall = networkService.getLatestVersionInfoDto();

		latestVersionInfoCall.enqueue(new Callback<LatestVersionInfoDto>() {
			@Override
			public void onResponse(Call<LatestVersionInfoDto> call, Response<LatestVersionInfoDto> response) {
				if (!response.isSuccessful()) {
					finish();
					System.exit(EXIT_SUCCESS_STATUS);
				}

				checkVersion(response.body());
			}

			@Override
			public void onFailure(Call<LatestVersionInfoDto> call, Throwable t) {
				finish();
				System.exit(EXIT_SUCCESS_STATUS);
			}
		});
    }

	private void startLauncher() {
		Intent intent = new Intent(this, MainActivity.class);

		if (getIntent().getExtras() != null && getIntent().getExtras().getBoolean(IS_AFTER_LOADING_KEY)){
			intent.putExtras(getIntent());
			intent.putExtra(IS_AFTER_LOADING_KEY, true);
		}

		startActivity(intent);
		finish();
	}

	private void checkVersion(LatestVersionInfoDto latestVersionInfo) {

		int currentVersion = getCurrentVersion();
		int latestVersion = Integer.parseInt(latestVersionInfo.getVersion());

		if (currentVersion >= latestVersion) {
			startLauncher();
			return;
		}

		MainUtils.setType(DownloadType.UPDATE_APK);
		MainUtils.LATEST_APK_INFO = latestVersionInfo;
		startActivity(new Intent(this, LoaderActivity.class));
	}

	private int getCurrentVersion(){
		PackageManager pm = this.getPackageManager();

		try {
			PackageInfo pInfo = pm.getPackageInfo(this.getPackageName(),0);
			return pInfo.versionCode;
		} catch (PackageManager.NameNotFoundException e1) {
			e1.printStackTrace();
		}

		finish();
		System.exit(EXIT_SUCCESS_STATUS);

		return 0;
	}

	public static boolean isOnline(Context context) {
		ConnectivityManager cm =
				(ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo netInfo = cm.getActiveNetworkInfo();

		return netInfo != null && netInfo.isConnectedOrConnecting();
	}
}