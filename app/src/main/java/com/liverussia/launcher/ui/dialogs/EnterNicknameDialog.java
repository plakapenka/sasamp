package com.liverussia.launcher.ui.dialogs;

import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.EditText;
import android.widget.TextView;

import androidx.appcompat.app.AlertDialog;

import com.google.android.material.dialog.MaterialAlertDialogBuilder;
import com.liverussia.cr.R;
import com.liverussia.launcher.domain.enums.NativeStorageElements;
import com.liverussia.launcher.ui.fragment.SettingsFragment;
import com.liverussia.launcher.storage.NativeStorage;
import com.liverussia.launcher.utils.Validator;

public class EnterNicknameDialog extends MaterialAlertDialogBuilder implements View.OnClickListener {

    private final SettingsFragment settingsFragment;

    private AlertDialog dialog;
    private EditText nicknameInput;
    private Animation animation;

    public EnterNicknameDialog(SettingsFragment settingsFragment) {
        super(settingsFragment.getActivity());
        this.settingsFragment = settingsFragment;
        createDialog();
    }

    private void createDialog() {
        View view = LayoutInflater.from(settingsFragment.getActivity()).inflate(R.layout.item_change_nik, null);
        setView(view);
        initViewElements(view);
        dialog = show();
        dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        setOnDialogCloseListener();
        animation = AnimationUtils.loadAnimation(getContext(), R.anim.button_click);
    }

    private void initViewElements(View view) {

        String nickname = NativeStorage.getClientProperty(NativeStorageElements.NICKNAME, settingsFragment.getActivity());

        nicknameInput = view.findViewById(R.id.nick_input);
        nicknameInput.setText(nickname);

        TextView saveButton = view.findViewById(R.id.nick_input_save_btn);
        saveButton.setOnClickListener(this);

        TextView cancelButton = view.findViewById(R.id.nick_input_cancel_btn);
        cancelButton.setOnClickListener(this);
    }

    private void setOnDialogCloseListener() {
        dialog.setOnDismissListener(alertDialog -> {
            String nickname = NativeStorage.getClientProperty(NativeStorageElements.NICKNAME, settingsFragment.getActivity());
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
            NativeStorage.addClientProperty(NativeStorageElements.NICKNAME, nickname, settingsFragment.getActivity());
            dialog.dismiss();
        }
    }
}
