package com.liverussia.launcher.service;

import android.app.Activity;

import androidx.appcompat.app.AppCompatActivity;

import com.liverussia.cr.core.Config;
import com.liverussia.launcher.enums.ServerInfo;

import java.io.File;

public interface ActivityService {

    void performHomeButtonAction(AppCompatActivity activity);

    void performPlayButtonAction(AppCompatActivity activity);

    void openSettingsActivity(AppCompatActivity activity);

    void openDonateActivity(AppCompatActivity activity);

    void showMessage(String message, Activity activity);

    void performSelectServerPlayButtonAction(ServerInfo serverInfo, AppCompatActivity activity);

    void performReinButtonAction(AppCompatActivity activity);

    void performYouTubeButtonAction(AppCompatActivity activity);

    void performVkButtonAction(AppCompatActivity activity);

    void performDiscordButtonAction(AppCompatActivity activity);

    void openRouletteActivity(AppCompatActivity activity);

    boolean isGameInstalled();

    void showBigMessage(String message, Activity activity);
}
