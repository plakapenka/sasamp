package com.liverussia.launcher.service.impl;

import android.app.Activity;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import com.liverussia.launcher.activity.LoaderActivity;
import com.liverussia.launcher.config.Config;
import com.liverussia.cr.core.Utils;
import com.liverussia.launcher.enums.ServerInfo;
import com.liverussia.launcher.enums.StorageElements;
import com.liverussia.launcher.messages.ErrorMessages;
import com.liverussia.launcher.messages.InfoMessages;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.storage.Storage;

import org.ini4j.Wini;

import java.io.File;
import java.io.IOException;

public class ActivityServiceImpl implements ActivityService {

    private static final String WINI_SECTION_NAME = "client";
    private static final String WINI_OPTION_NAME = "server";

    @Override
    public void performSelectServerPlayButtonAction(ServerInfo serverInfo, AppCompatActivity activity) {
        boolean isNicknameInput = Storage.isPropertyExist(StorageElements.NICKNAME.getValue(), activity);

        if (!isNicknameInput) {
            openSettingsActivity(activity);
            showMessage(ErrorMessages.INPUT_NICKNAME_BEFORE_SERVER_CONNECT.getText(), activity);
            return;
        }

        configureGameParams(serverInfo, activity);

        if (isGameInstalled()) {
            startClient(activity);
        } else {
            startInstallGame(activity);
        }
    }

    @Override
    public void performReinButtonAction(AppCompatActivity activity) {
        uninstallFilesGame(activity);
        startInstallGame(activity);
    }

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
    public void openRouletteActivity(AppCompatActivity activity) {

    }

    @Override
    public void performHomeButtonAction(AppCompatActivity activity) {

    }

    @Override
    public void performPlayButtonAction(AppCompatActivity activity) {

    }

    @Override
    public void openSettingsActivity(AppCompatActivity activity) {

    }

    @Override
    public void openDonateActivity(AppCompatActivity activity) {

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

    private void configureGameParams(ServerInfo serverInfo, AppCompatActivity activity) {
        try {
            File f = new File(Config.GAME_PATH + Config.GAME_SETTINGS_DIRECTORY);

            if(!f.exists()) {
                f.createNewFile();
                f.mkdirs();
            }

            Wini wini = new Wini(new File(Config.GAME_PATH + Config.GAME_SETTINGS_DIRECTORY));
            wini.put(WINI_SECTION_NAME, WINI_OPTION_NAME, serverInfo.getIndex());
            wini.store();

            String message = InfoMessages.createConnectToSelectServerMessage(serverInfo.getName());
            showMessage(message, activity);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public boolean isGameInstalled() {
        String GetGamePath = Config.GAME_PATH + "texdb/gta3.img";
        File file = new File(GetGamePath);
        return file.exists();
    }

    private void startInstallGame(AppCompatActivity activity) {
        Intent intent = new Intent(activity, LoaderActivity.class);
        Utils.setType(0);
        intent.putExtras(activity.getIntent());
        activity.startActivity(intent);
    }

    private void startClient(AppCompatActivity activity) {
        Intent intent = new Intent(activity, com.liverussia.cr.core.GTASA.class);
        intent.putExtras(activity.getIntent());
        System.out.println("StartActivity GTASA.class");
        activity.startActivity(intent);
        activity.finish();
    }

    private boolean uninstallFilesGame(AppCompatActivity activity) {
        try {
            File dir = new File(Config.GAME_DIR+"com.liverussia.cr/");
            if (dir.isDirectory()) {
                String[] children = dir.list();
                for (int i=0; i<children.length; i++) {
                    boolean success = deleteDir(new File(dir, children[i]));
                    if (!success) {
                    }
                }
                return false;
            }
        }
        catch (Exception e) {
            System.out.println(e);
        }

        AlertDialog.Builder builder = new AlertDialog.Builder(activity);
        builder.setTitle("Файлы игры удалены").
                setMessage("Файлы игры успешно удалены с вашего телефона!")
                //.setIcon(R.drawable.)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.cancel();
                    }});
        return false;
    }

    public static boolean deleteDir(File dir) {
        if (dir.isDirectory()) {
            for (File child : dir.listFiles()) {
                boolean success = deleteDir(child);
                if (!success) {
                    return false;
                }
            }
        }
        return dir.delete();
    }
}
