package com.liverussia.launcher.ui.fragment;

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
import com.liverussia.launcher.async.dto.response.FileInfo;
import com.liverussia.launcher.async.task.CacheChecker;
import com.liverussia.launcher.ui.activity.LoaderActivity;
import com.liverussia.launcher.utils.MainUtils;
import com.liverussia.launcher.ui.dialogs.EnterNicknameDialogBuilder;
import com.liverussia.launcher.domain.enums.DownloadType;
import com.liverussia.launcher.domain.enums.NativeStorageElements;
import com.liverussia.launcher.domain.messages.InfoMessages;
import com.liverussia.launcher.utils.FileUtils;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;
import com.liverussia.launcher.storage.NativeStorage;
import com.liverussia.launcher.service.ActivityService;

import org.springframework.util.CollectionUtils;

import java.io.File;
import java.util.Arrays;
import java.util.List;

import static com.liverussia.launcher.config.Config.SETTINGS_FILE_PATH;

public class SettingsFragment extends Fragment implements View.OnClickListener {

    private final static int GAME_DIRECTORY_EMPTY_SIZE = 0;

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
        inflate.findViewById(R.id.validateCache).setOnClickListener(this);
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
            case R.id.validateCache:
                view.startAnimation(animation);
                performValidateCacheButtonAction();
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
        MainUtils.setType(DownloadType.LOAD_ALL_CACHE);

        startActivity(new Intent(getActivity(), com.liverussia.launcher.ui.activity.LoaderActivity.class));
    }

    private void performValidateCacheButtonAction() {
        File gameDirectory = new File(getActivity().getExternalFilesDir(null).toString());

        if (gameDirectory.list() != null && gameDirectory.list().length > GAME_DIRECTORY_EMPTY_SIZE) {
            CacheChecker cacheChecker = new CacheChecker(getActivity());
            cacheChecker.setOnAsyncSuccessListener(this::doAfterCacheChecked);
            cacheChecker.validateCache();
        } else {
            MainUtils.setType(DownloadType.LOAD_ALL_CACHE);
            startActivity(new Intent(getActivity(), LoaderActivity.class));
        }
    }

    private void doAfterCacheChecked(FileInfo[] fileToReloadArray) {

        List<FileInfo> filesToReloadList = Arrays.asList(fileToReloadArray);

        if (CollectionUtils.isEmpty(filesToReloadList)) {
            activityService.showMessage(InfoMessages.GAME_FILES_VALID.getText(), getActivity());
        } else {
            validateCache(filesToReloadList);
        }
    }

    private void validateCache(List<FileInfo> filesToReloadList) {
        MainUtils.FILES_TO_RELOAD = filesToReloadList;
        MainUtils.setType(DownloadType.RELOAD_OR_ADD_PART_OF_CACHE);
        startActivity(new Intent(getActivity(), LoaderActivity.class));
    }

    private void performResetSettingsButtonAction() {
        if (!activityService.isGameFileInstall(getActivity(), SETTINGS_FILE_PATH)) {
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