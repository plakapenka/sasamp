package com.liverussia.launcher.activity.dialogs;

import android.view.LayoutInflater;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.EditText;
import android.widget.TextView;

import androidx.appcompat.app.AlertDialog;

import com.google.android.material.dialog.MaterialAlertDialogBuilder;
import com.liverussia.cr.R;
import com.liverussia.launcher.enums.StorageElements;
import com.liverussia.launcher.fragment.SettingsFragment;
import com.liverussia.launcher.storage.Storage;
import com.liverussia.launcher.utils.Validator;

public class EnterNicknameDialogBuilder extends MaterialAlertDialogBuilder implements View.OnClickListener {

    private final SettingsFragment settingsFragment;

    private AlertDialog dialog;
    private EditText nicknameInput;
    private Animation animation;

    public EnterNicknameDialogBuilder(SettingsFragment settingsFragment) {
        super(settingsFragment.getActivity());
        this.settingsFragment = settingsFragment;
        createDialog();
    }

    //TODO белые углы и диалога
    private void createDialog() {
        View view = LayoutInflater.from(settingsFragment.getActivity()).inflate(R.layout.item_change_nik, null);
        setView(view);
        initViewElements(view);
        dialog = show();

        setOnDialogCloseListener();
        animation = AnimationUtils.loadAnimation(getContext(), R.anim.button_click);
    }

    private void initViewElements(View view) {

        String nickname = Storage.getProperty(StorageElements.NICKNAME.getValue(), settingsFragment.getActivity());

        nicknameInput = view.findViewById(R.id.nick_input);
        nicknameInput.setText(nickname);

        TextView saveButton = view.findViewById(R.id.nick_input_save_btn);
        saveButton.setOnClickListener(this);

        TextView cancelButton = view.findViewById(R.id.nick_input_cancel_btn);
        cancelButton.setOnClickListener(this);
    }

    private void setOnDialogCloseListener() {
        dialog.setOnDismissListener(alertDialog -> {
            String nickname = Storage.getProperty(StorageElements.NICKNAME.getValue(), settingsFragment.getActivity());
            settingsFragment.updateNicknameField(nickname);
        });
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.nick_input_save_btn:
                view.startAnimation(animation);
                performSaveButtonAction();
                break;
            case R.id.nick_input_cancel_btn:
                view.startAnimation(animation);
                dialog.dismiss();
                break;
            default:
                break;
        }
    }

    private void performSaveButtonAction() {
        String nickname = nicknameInput.getText().toString();

        if (Validator.isValidNickname(nickname, settingsFragment.getActivity())) {
            Storage.addProperty(StorageElements.NICKNAME.getValue(), nickname, settingsFragment.getActivity());
            dialog.dismiss();
        }
    }
}
