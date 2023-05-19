package com.ssmp.launcher.ui.dialogs;

import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.TextView;

import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.FragmentActivity;

import com.google.android.material.dialog.MaterialAlertDialogBuilder;
import com.ssmp.cr.R;
import com.ssmp.launcher.domain.messages.InfoMessage;
import com.ssmp.launcher.ui.dialogs.listener.OnDialogCloseListener;

public class ConfirmDialog extends MaterialAlertDialogBuilder implements View.OnClickListener {

    private final InfoMessage title;

    private AlertDialog dialog;
    private Animation animation;

    private OnDialogCloseListener<Boolean> onDialogCloseListener;

    public ConfirmDialog(FragmentActivity fragmentActivity, InfoMessage title) {
        super(fragmentActivity);
        this.title = title;
    }

    public void createDialog() {
        View view = LayoutInflater.from(getContext()).inflate(R.layout.confirm_dialog, null);
        setView(view);
        initViewElements(view);
        dialog = show();
        dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        animation = AnimationUtils.loadAnimation(getContext(), R.anim.button_click);
    }

    private void initViewElements(View view) {
        view.findViewById(R.id.confirm_ok).setOnClickListener(this);
        view.findViewById(R.id.confirm_not).setOnClickListener(this);

        TextView textView = view.findViewById(R.id.confirm_text_view);
        textView.setText(title.getText());
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.confirm_ok:
                view.startAnimation(animation);
                onDialogCloseListener.onClose(true);
                dialog.dismiss();
                break;
            case R.id.confirm_not:
                view.startAnimation(animation);
                onDialogCloseListener.onClose(false);
                dialog.dismiss();
                break;
            default:
                break;
        }
    }

    public void setOnDialogCloseListener(OnDialogCloseListener<Boolean> onDialogCloseListener) {
        this.onDialogCloseListener = onDialogCloseListener;
    }

}
