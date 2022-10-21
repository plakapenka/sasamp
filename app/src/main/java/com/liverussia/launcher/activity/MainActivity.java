package com.liverussia.launcher.activity;

import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.os.Build;

import android.widget.*;
import android.graphics.PorterDuff;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import android.content.Intent;

import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentTransaction;

import android.view.View.OnClickListener;
import android.view.View;
import android.view.animation.AnimationUtils;
import android.view.animation.Animation;

import com.liverussia.cr.R;
import com.liverussia.cr.core.Config;
import com.liverussia.cr.gui.util.Utils;
import com.liverussia.launcher.fragment.MonitoringFragment;
import com.liverussia.launcher.fragment.ForumFragment;
import com.liverussia.launcher.fragment.DonateFragment;
import com.liverussia.launcher.fragment.SettingsFragment;

import java.io.File;

import java.util.*;

public class MainActivity extends AppCompatActivity {
    
	public LinearLayout donateButton;
    public DonateFragment donateFragment;
    public ImageView donateImage;
    public TextView donateTV;
    public LinearLayout monitoringButton;
    public MonitoringFragment monitoringFragment;
    public ImageView monitoringImage;
    public TextView monitoringTV;
    public LinearLayout playButton;
    public ImageView playImage;
    public LinearLayout forumButton;
    public ForumFragment forumFragment;
    public ImageView forumImage;
    public TextView forumTV;
    public LinearLayout settingsButton;
    public SettingsFragment settingsFragment;  
    public ImageView settingsImage;
    public TextView settingsTV;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //setContentView(R.layout.pre_death);
       // setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);


		Animation animation = AnimationUtils.loadAnimation(this, R.anim.button_click);

		monitoringTV = (TextView) findViewById(R.id.monitoringTV);
        settingsTV = (TextView) findViewById(R.id.settingsTV);
        forumTV = (TextView) findViewById(R.id.rouletteBut);
        donateTV = (TextView) findViewById(R.id.donateTV);

        monitoringImage = (ImageView) findViewById(R.id.monitoringImage);
        settingsImage = (ImageView) findViewById(R.id.settingsImage);
        forumImage = (ImageView) findViewById(R.id.forumImage);
        donateImage = (ImageView) findViewById(R.id.donateImage);
        playImage = (ImageView) findViewById(R.id.playImage);

        monitoringButton = (LinearLayout) findViewById(R.id.monitoringButton);
        settingsButton = (LinearLayout) findViewById(R.id.settingsButton);
        forumButton = (LinearLayout) findViewById(R.id.rouletteButton);
        donateButton = (LinearLayout) findViewById(R.id.donateButton);
        playButton = (LinearLayout) findViewById(R.id.playButton);

		monitoringFragment = new MonitoringFragment();
        settingsFragment = new SettingsFragment();
        forumFragment = new ForumFragment();
        donateFragment = new DonateFragment();

	    replaceFragment(monitoringFragment);

		monitoringButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                v.startAnimation(animation);
                onClickMonitoring();
            }
        });

        settingsButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                v.startAnimation(animation);
                onClickSettings();
            }
        });

        forumButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
              v.startAnimation(animation);
			  onClickForum();
            }
        });

        donateButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
               v.startAnimation(animation);
			   onClickDonate();
            }
        });

        playButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
               v.startAnimation(animation);
			   startTimer();
            }
        });

    }
	
	public void onClickPlay() {
        if(IsGameInstalled()) {
            startActivity(new Intent(getApplicationContext(), com.liverussia.cr.core.GTASA.class));
		} else {
		    startActivity(new Intent(getApplicationContext(), LoaderActivity.class));
		}
    }

    public void onClickSettings() {
            setTextColor(settingsButton, settingsTV, settingsImage);
            replaceFragment(this.settingsFragment);
    }

    public void onClickForum() {
            setTextColor(forumButton, forumTV, forumImage);
            replaceFragment(forumFragment);
    }

    public void onClickDonate() {
            setTextColor(donateButton, donateTV, donateImage);
            replaceFragment(donateFragment);
    }
    
	public void onClickMonitoring() {
            setTextColor(monitoringButton, monitoringTV, monitoringImage);
            replaceFragment(monitoringFragment);
    }
	
    public void setTextColor(LinearLayout linearLayout, TextView textView, ImageView imageView) {

        monitoringTV.setTextColor(getResources().getColor(R.color.menuTextDisable));
        settingsTV.setTextColor(getResources().getColor(R.color.menuTextDisable));
        forumTV.setTextColor(getResources().getColor(R.color.menuTextDisable));
        donateTV.setTextColor(getResources().getColor(R.color.menuTextDisable));
        monitoringImage.setColorFilter(getResources().getColor(R.color.menuTextDisable), PorterDuff.Mode.SRC_IN);
        settingsImage.setColorFilter(getResources().getColor(R.color.menuTextDisable), PorterDuff.Mode.SRC_IN);
        forumImage.setColorFilter(getResources().getColor(R.color.menuTextDisable), PorterDuff.Mode.SRC_IN);
        donateImage.setColorFilter(getResources().getColor(R.color.menuTextDisable), PorterDuff.Mode.SRC_IN);

        textView.setTextColor(getResources().getColor(R.color.menuTextEnable));
        imageView.setColorFilter(getResources().getColor(R.color.menuTextEnable), PorterDuff.Mode.SRC_IN);
    }

    public void replaceFragment(Fragment fragment) {
        FragmentTransaction beginTransaction = getSupportFragmentManager().beginTransaction();
        beginTransaction.replace(R.id.container, fragment);
        beginTransaction.commit();
    }
	
	public boolean isRecordAudioPermissionGranted() {
        if (Build.VERSION.SDK_INT < 23 || checkSelfPermission("android.permission.RECORD_AUDIO") == 0) {
            return true;
        }
        ActivityCompat.requestPermissions(this, new String[]{"android.permission.RECORD_AUDIO"}, 2);
        return false;
    }

    public boolean isStoragePermissionGranted() {
        if (Build.VERSION.SDK_INT < 23 || checkSelfPermission("android.permission.WRITE_EXTERNAL_STORAGE") == 0) {
            return true;
        }
        ActivityCompat.requestPermissions(this, new String[]{"android.permission.WRITE_EXTERNAL_STORAGE"}, 1);
        return false;
    }
	
	private boolean IsGameInstalled()
    {
        String CheckFile = Config.GAME_PATH + "texdb/gta3.img";
        File file = new File(CheckFile);
        return file.exists();
    }
	
	private void startTimer()
    {
        Timer t = new Timer();
        t.schedule(new TimerTask(){

            @Override
            public void run() {
                onClickPlay();
            }
        }, 200L);
    }
	
	public void onDestroy() {
        super.onDestroy();
    }

    public void onRestart() {
        super.onRestart();
    }
	
	public boolean checkValidNick(){
		EditText nick = (EditText) findViewById(R.id.editText);
		if(nick.getText().toString().isEmpty()) {
			Toast.makeText(this, "Введите ник", Toast.LENGTH_SHORT).show();
			return false;
		}
		if(!(nick.getText().toString().contains("_"))){
			Toast.makeText(this, "Ник должен содержать символ \"_\"", Toast.LENGTH_SHORT).show();
			return false;
		}
		if(nick.getText().toString().length() < 4){
			Toast.makeText(this, "Длина ника должна быть не менее 4 символов", Toast.LENGTH_SHORT).show();
			return false;
		}
		return true;
	}
} 