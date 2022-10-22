package com.liverussia.cr.gui;

import android.app.Activity;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.view.KeyEvent;
import android.view.animation.AnimationUtils;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;
import com.liverussia.cr.core.Config;
import com.liverussia.cr.gui.util.Utils;
import com.nvidia.devtech.CustomEditText;
import com.nvidia.devtech.NvEventQueueActivity;

import org.ini4j.Wini;

import java.io.File;
import java.io.IOException;

public class AuthorizationManager {

    private FrameLayout br_authorization_layout;
    private ConstraintLayout auth_right1;
    private ConstraintLayout auth_right2;
    private ConstraintLayout auth_back;
    private ImageView auth_play;
    private ImageView auth_btn;
    private CustomEditText auth_password;
    private TextView auth_infotitle;
    private TextView auth_info;
    private TextView auth_nick;

    public AuthorizationManager(Activity activity){
        br_authorization_layout = activity.findViewById(R.id.br_authorization_layout);
        auth_right1 = activity.findViewById(R.id.auth_right1);
        auth_right2 = activity.findViewById(R.id.auth_right2);
        auth_back = activity.findViewById(R.id.auth_back);
        auth_play = activity.findViewById(R.id.auth_play);
        auth_btn = activity.findViewById(R.id.auth_btn);
        auth_password = activity.findViewById(R.id.auth_password);
        auth_infotitle = activity.findViewById(R.id.auth_infotitle);
        auth_info = activity.findViewById(R.id.auth_info);
        auth_nick = activity.findViewById(R.id.auth_nick);

        auth_play.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            if (auth_password.getText().length() > 5)
            {
                NvEventQueueActivity.getInstance().onLoginClick(auth_password.getText().toString());
            }
        });

        auth_back.setOnClickListener(view -> {
            Utils.HideLayout(auth_right2, false);
            Utils.ShowLayout(auth_right1, false);
            auth_infotitle.setText("Выберите способ авторизации!");
            auth_info.setText("Информация: если этот аккаунт является не вашим, то выйдите из игры и смените игровое имя на новое!");
        });

        auth_btn.setOnClickListener(view -> {
            Utils.HideLayout(auth_right1, false);
            Utils.ShowLayout(auth_right2, false);
            auth_infotitle.setText("Для начала игры заполните все поля");
            auth_info.setText("• Пароль должен состоять от 6 до 16 символов\n\n• Пароль чувствителен к регистру");
        });

        auth_password.addTextChangedListener(new TextWatcher() {
            @Override
            public void afterTextChanged(Editable s) {}

            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {}

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                if (auth_password.getText().length() > 5)
                {
                    auth_play.setBackgroundResource(R.drawable.auth_btn_play);
                }
                else
                {
                    auth_play.setBackgroundResource(R.drawable.auth_btn_play_none);
                }
            }
        });

        Utils.HideLayout(br_authorization_layout, false);
    }

    public void Show(String nick, int id) {
        String strnickid = String.format("%s [%d]", nick, id);
        auth_nick.setText(strnickid);
        Utils.ShowLayout(br_authorization_layout, false);
    }

    public void Hide() {
        Utils.HideLayout(br_authorization_layout, true);
    }
}