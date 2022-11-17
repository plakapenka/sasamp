package com.liverussia.launcher.fragment;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.*;

import androidx.fragment.app.Fragment;

import com.liverussia.cr.R;
import com.liverussia.cr.core.DownloadUtils;
import com.liverussia.launcher.activity.dialogs.EnterNicknameDialogBuilder;
import com.liverussia.launcher.enums.DownloadType;
import com.liverussia.launcher.enums.NativeStorageElements;
import com.liverussia.launcher.messages.InfoMessages;
import com.liverussia.launcher.other.FileUtils;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;
import com.liverussia.launcher.storage.NativeStorage;
import com.liverussia.launcher.service.ActivityService;

import java.io.File;

import static com.liverussia.launcher.config.Config.SETTINGS_FILE_PATH;

public class SettingsFragment extends Fragment implements View.OnClickListener{

    private Animation animation;
    private TextView nicknameField;
    private ActivityService activityService;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        activityService = new ActivityServiceImpl();

        View inflate = inflater.inflate(R.layout.fragment_settings, container, false);

        animation = AnimationUtils.loadAnimation(getContext(), R.anim.button_click);

        this.getActivity().setTheme(R.style.AppBaseTheme);

        nicknameField = inflate.findViewById(R.id.nick_edit);
        nicknameField.setOnClickListener(this);

        inflate.findViewById(R.id.youtubeButton).setOnClickListener(this);
        inflate.findViewById(R.id.vkButton).setOnClickListener(this);
        inflate.findViewById(R.id.discordButton).setOnClickListener(this);
        inflate.findViewById(R.id.resetSettings).setOnClickListener(this);
        inflate.findViewById(R.id.reinstallGame).setOnClickListener(this);
        inflate.findViewById(R.id.telegramButton).setOnClickListener(this);

        initUserData();

        return inflate;
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.youtubeButton:
                view.startAnimation(animation);
                performYouTubeButtonAction();
                break;
            case R.id.vkButton:
                view.startAnimation(animation);
                performVkButtonAction();
                break;
            case R.id.discordButton:
                view.startAnimation(animation);
                performDiscordButtonAction();
                break;
            case R.id.telegramButton:
                view.startAnimation(animation);
                performTelegramButtonAction();
                break;
            case R.id.resetSettings:
                view.startAnimation(animation);
                performResetSettingsButtonAction();
                break;
            case R.id.reinstallGame:
                view.startAnimation(animation);
                performReinstallGameButtonAction();
                break;
            case R.id.nick_edit:
                performNickEditFieldOnClickAction();
                break;
            default:
                break;
        }
    }

    private void performNickEditFieldOnClickAction() {
        new EnterNicknameDialogBuilder(this);
    }

    private void performReinstallGameButtonAction() {
        File gameDirectory = (new File(getActivity().getExternalFilesDir(null).toString()));
        FileUtils.delete(gameDirectory);
        DownloadUtils.setType(DownloadType.LOAD_ALL_CACHE);

        startActivity(new Intent(getActivity(), com.liverussia.launcher.activity.LoaderActivity.class));
    }

    private void performResetSettingsButtonAction() {
        if (!activityService.isGameInstalled()) {
            activityService.showMessage(InfoMessages.INSTALL_GAME_FIRST.getText(), getActivity());
            return;
        }

        File settingsFile = (new File(getActivity().getExternalFilesDir(null) + SETTINGS_FILE_PATH));

        if (settingsFile.exists()) {
            settingsFile.delete();
            activityService.showMessage(InfoMessages.SUCCESSFULLY_SETTINGS_RESET.getText(), getActivity());
        } else {
            activityService.showMessage(InfoMessages.SETTINGS_ALREADY_DEFAULT.getText(), getActivity());
        }

    }

    private void performDiscordButtonAction() {
        startActivity(new Intent("android.intent.action.VIEW", Uri.parse("https://discord.com")));
    }

    private void performVkButtonAction() {
        startActivity(new Intent("android.intent.action.VIEW", Uri.parse("https://vk.com")));
    }

    private void performYouTubeButtonAction() {
        startActivity(new Intent("android.intent.action.VIEW", Uri.parse("https://youtube.com")));
    }

    private void performTelegramButtonAction() {
        startActivity(new Intent("android.intent.action.VIEW", Uri.parse("https://telegram.org")));
    }

    private void initUserData() {
        String nickname = NativeStorage.getClientProperty(NativeStorageElements.NICKNAME, this.getActivity());
        nicknameField.setText(nickname);
    }

    public void updateNicknameField(String nickname) {
        this.nicknameField.setText(nickname);
    }
}