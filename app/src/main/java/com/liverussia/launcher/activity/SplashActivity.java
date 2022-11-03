package com.liverussia.launcher.activity;

import android.Manifest;
import android.content.Intent;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.view.View;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.liverussia.cr.R;

import java.util.Timer;
import java.util.TimerTask;

import java.util.ArrayList;
import java.util.List;

import com.liverussia.launcher.error.apiException.ErrorContainer;
import com.liverussia.launcher.model.News;

import com.liverussia.launcher.model.Servers;
import com.liverussia.launcher.other.NetworkService;
import com.liverussia.launcher.other.Lists;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

public class SplashActivity extends AppCompatActivity{

	public static ArrayList<Servers> slist;
	
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash);

		getWindow().getDecorView().setSystemUiVisibility(
				View.SYSTEM_UI_FLAG_IMMERSIVE
						| View.SYSTEM_UI_FLAG_LAYOUT_STABLE
						| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
						| View.SYSTEM_UI_FLAG_FULLSCREEN
						| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);

		Lists.SERVERS = new ArrayList<>();
		Lists.NEWS = new ArrayList<>();

		Retrofit retrofit = new Retrofit.Builder()
				.baseUrl("http://vbd.fdv.dd/")
				.addConverterFactory(GsonConverterFactory.create())
				.build();

		NetworkService sNetworkService = retrofit.create(NetworkService.class);

		Call<List<Servers>> scall = sNetworkService.getServers();

		scall.enqueue(new Callback<List<Servers>>() {
			@Override
			public void onResponse(Call<List<Servers>> call, Response<List<Servers>> response) {

				List<Servers> servers = response.body();

				for (Servers server : servers) {
					Lists.SERVERS.add(new Servers(server.getColor(), server.getServerID(), server.getDopname(), server.getname(), server.getOnline(), server.getmaxOnline()));
				}
			}

			@Override
			public void onFailure(Call<List<Servers>> call, Throwable t) {
				Toast.makeText(getApplicationContext(), ErrorContainer.SERVER_CONNECT_ERROR.getMessage(), Toast.LENGTH_SHORT).show();
			}
		});

		Call<List<News>> ncall = sNetworkService.getNews();

		ncall.enqueue(new Callback<List<News>>() {
			@Override
			public void onResponse(Call<List<News>> call, Response<List<News>> response) {

				List<News> news = response.body();

				for (News storie : news) {
					Lists.NEWS.add(new News(storie.getImageUrl(), storie.getTitle(), storie.getUrl()));
				}
			}

			@Override
			public void onFailure(Call<List<News>> call, Throwable t) {
				System.out.println("sfjksjdf");
			}
		});
		
        if (Build.VERSION.SDK_INT >= 23) {
            if (checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED
                    || checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED || checkSelfPermission(Manifest.permission.RECORD_AUDIO) == PackageManager.PERMISSION_DENIED) {
				//requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.RECORD_AUDIO}, 1000);
				startActivity(new Intent(this, PolicyActivity.class));
				finish();
			} else {
				startTimer();
			}
        } else startTimer();
	}
	
    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 1000) {
            startLauncher();
        }
    }

    private void startLauncher() {
        startActivity(new Intent(this, MainActivity.class));
        finish();
    }
	
	public static boolean isOnline(Context context) {
        ConnectivityManager cm =
                (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo netInfo = cm.getActiveNetworkInfo();

		return netInfo != null && netInfo.isConnectedOrConnecting();
	}
	
	private void startTimer() {
        Timer t = new Timer();
        t.schedule(new TimerTask(){
            @Override
            public void run() {
                startLauncher();
            }
        }, 1000L);
    }
}