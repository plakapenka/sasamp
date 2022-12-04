package com.liverussia.launcher.service;

import android.app.Activity;

import androidx.appcompat.app.AppCompatActivity;

public interface ActivityService {

    void showMessage(String message, Activity activity);

    void performYouTubeButtonAction(AppCompatActivity activity);

    void performVkButtonAction(AppCompatActivity activity);

    void performDiscordButtonAction(AppCompatActivity activity);

    boolean isGameFileInstall(Activity activity, String filePath);

    void showBigMessage(String message, Activity activity);
}
