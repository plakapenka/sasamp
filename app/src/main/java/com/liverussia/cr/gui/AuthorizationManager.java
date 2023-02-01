package com.liverussia.cr.gui;

import android.app.Activity;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.google.android.material.button.MaterialButton;
import com.google.android.material.switchmaterial.SwitchMaterial;
import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.nvidia.devtech.NvEventQueueActivity;

public class AuthorizationManager {

    private FrameLayout br_authorization_layout;
    private ConstraintLayout auth_right1;
    private ConstraintLayout auth_right2;
    private ConstraintLayout auth_back;
    private MaterialButton auth_play;
    private ImageView auth_btn;
    private EditText auth_password;
    private TextView auth_infotitle;
    private TextView auth_info;
    private TextView auth_nick;
    private SwitchMaterial switcher_autologin;
    private TextView recovery_password;
    private ConstraintLayout autologin_main_help;
    private MaterialButton autologin_help_close;
    private ImageView autologin_open_help;

    native void ToggleAutoLogin(boolean toggle);
    native void ClickRecoveryPass();

    public AuthorizationManager(Activity activity){
        autologin_open_help = activity.findViewById(R.id.autologin_open_help);
        autologin_help_close = activity.findViewById(R.id.autologin_help_close);
        autologin_main_help = activity.findViewById(R.id.autologin_main_help);
        recovery_password = activity.findViewById(R.id.recovery_password);
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
        switcher_autologin = activity.findViewById(R.id.switcher_autologin);

        autologin_open_help.setOnClickListener(view -> {
            autologin_main_help.setVisibility(View.VISIBLE);
        });
        autologin_help_close.setOnClickListener(view -> {
            autologin_main_help.setVisibility(View.GONE);
        });
        switcher_autologin.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                ToggleAutoLogin(b);
            }
        });
        recovery_password.setOnClickListener(view -> {
            ClickRecoveryPass();
        });

        auth_play.setOnClickListener(view -> {
           // view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
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
            auth_infotitle.setText("Введите пароль, чтобы войти в игру.");
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
                    auth_play.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#fbc02d")));
                }
                else
                {
                    auth_play.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#004261")));
                }
            }
        });

        Utils.HideLayout(br_authorization_layout, false);
    }

    public void Show(String nick, int id, boolean ip_match, boolean toggle_autologin, boolean email_acvive) {
        if(email_acvive) {
            recovery_password.setVisibility(View.VISIBLE);
        }
        else{
            recovery_password.setVisibility(View.GONE);
        }
        switcher_autologin.setChecked(toggle_autologin);
        String strnickid = String.format("%s [%d]", nick, id);
        auth_nick.setText(strnickid);
        Utils.ShowLayout(br_authorization_layout, false);
    }

    public void Hide() {
        Utils.HideLayout(br_authorization_layout, true);
    }
}