package com.liverussia.launcher.ui.dialogs;

import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.EditText;
import android.widget.TextView;

import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.FragmentActivity;

import com.google.android.material.dialog.MaterialAlertDialogBuilder;
import com.liverussia.cr.R;
import com.liverussia.launcher.domain.messages.InfoMessage;
import com.liverussia.launcher.ui.dialogs.listener.OnDialogCloseListener;

import org.apache.commons.lang3.StringUtils;

public class EnterLockedServerPasswordDialog extends MaterialAlertDialogBuilder implements View.OnClickListener {

    private AlertDialog dialog;
    private EditText passwordField;
    private Animation animation;

    private OnDialogCloseListener<String> onDialogCloseListener;

    public EnterLockedServerPasswordDialog(Context context) {
        super(context);
    }

    public void createDialog() {
        View view = LayoutInflater.from(getContext()).inflate(R.layout.enter_password_locked_server, null);
        setView(view);
        initViewElements(view);
        dialog = show();
        dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        animation = AnimationUtils.loadAnimation(getContext(), R.anim.button_click);
    }

    private void initViewElements(View view) {
        view.findViewById(R.id.pass_input_cancel_btn).setOnClickListener(this);
        view.findViewById(R.id.pass_input_save_btn).setOnClickListener(this);

        passwordField = view.findViewById(R.id.password_input);
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.pass_input_save_btn:
                view.startAnimation(animation);
                onDialogCloseListener.onClose(passwordField.getText().toString());
                dialog.dismiss();
                break;
            case R.id.pass_input_cancel_btn:
                view.startAnimation(animation);
                onDialogCloseListener.onClose(StringUtils.EMPTY);
                dialog.dismiss();
                break;
            default:
                break;
        }
    }

    public void setOnDialogCloseListener(OnDialogCloseListener<String> onDialogCloseListener) {
        this.onDialogCloseListener = onDialogCloseListener;
    }

}
