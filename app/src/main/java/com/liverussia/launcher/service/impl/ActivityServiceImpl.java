package com.liverussia.launcher.service.impl;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.liverussia.launcher.config.Config;
import com.liverussia.launcher.service.ActivityService;

import java.io.File;

public class ActivityServiceImpl implements ActivityService {

    @Override
    public void performYouTubeButtonAction(AppCompatActivity activity) {
        Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse(Config.YOU_TUBE_URI));
        activity.startActivity(intent);
    }

    @Override
    public void performVkButtonAction(AppCompatActivity activity) {
        Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse(Config.VK_URI));
        activity.startActivity(intent);
    }

    @Override
    public void performDiscordButtonAction(AppCompatActivity activity) {
        Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse(Config.DISCORD_URI));
        activity.startActivity(intent);
    }

    @Override
    public void showMessage(String message, Activity activity) {
        Toast.makeText(activity, message, Toast.LENGTH_SHORT)
                .show();
    }

    @Override
    public void showBigMessage(String message, Activity activity) {
        Toast.makeText(activity, message, Toast.LENGTH_LONG)
                .show();
    }

    @Override
    public boolean isGameInstalled(Activity activity) {
        String GetGamePath = activity.getExternalFilesDir(null).toString() + "texdb/gta3.img";
        File file = new File(GetGamePath);
        return file.exists();
    }
}
