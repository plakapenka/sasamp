package com.liverussia.cr.gui;

import android.app.Activity;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.google.android.material.button.MaterialButton;
import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.nvidia.devtech.NvEventQueueActivity;

public class RegistrationManager {

    private FrameLayout br_registration_layout;
    private ConstraintLayout reg_right1;
    private ConstraintLayout reg_right2;
    private ConstraintLayout reg_back;
    private ConstraintLayout registration_layout;
    private ConstraintLayout sex_layout;
    private ConstraintLayout skin_layout;
    private ImageView choose_skin_notf;
    private ImageView choose_skin_left;
    private ImageView choose_skin_right;
    private ImageView choose_skin_btn;
    private ImageView choose_skin_notf_close;
    private ImageView choosesex_male;
    private ImageView choosesex_female;
    private MaterialButton choosesex_btn;
    private MaterialButton reg_play;
    private ImageView reg_btn;
    private ImageView reg_skin;
    private EditText reg_password;
    private EditText reg_passwordtwo;
    private EditText reg_mail;
    private TextView reg_infotitle;
    private TextView reg_info;
    private TextView reg_nick;

    private String MailString = "";
    private int choosesex;

    public RegistrationManager(Activity activity){
        br_registration_layout = activity.findViewById(R.id.br_registration_layout);
        registration_layout = activity.findViewById(R.id.registration_layout);

        skin_layout = activity.findViewById(R.id.skin_layout);
        skin_layout.setVisibility(View.GONE);
        choose_skin_notf = activity.findViewById(R.id.choose_skin_notf);
        choose_skin_left = activity.findViewById(R.id.choose_skin_left);
        choose_skin_right = activity.findViewById(R.id.choose_skin_right);
        choose_skin_btn = activity.findViewById(R.id.choose_skin_btn);
        choose_skin_notf_close = activity.findViewById(R.id.choose_skin_notf_close);
        sex_layout = activity.findViewById(R.id.sex_layout);
        sex_layout.setVisibility(View.GONE);
        choosesex_male = activity.findViewById(R.id.choosesex_male);
        choosesex_female = activity.findViewById(R.id.choosesex_female);
        choosesex_btn = activity.findViewById(R.id.choosesex_btn);
        reg_right1 = activity.findViewById(R.id.reg_right1);
        reg_right2 = activity.findViewById(R.id.reg_right2);
        reg_back = activity.findViewById(R.id.reg_back);
        reg_play = activity.findViewById(R.id.reg_play);
        reg_btn = activity.findViewById(R.id.reg_btn);
        reg_skin = activity.findViewById(R.id.reg_skin);
        reg_password = activity.findViewById(R.id.reg_password);
        reg_passwordtwo = activity.findViewById(R.id.reg_passwordtwo);
        reg_mail = activity.findViewById(R.id.reg_mail);
        reg_infotitle = activity.findViewById(R.id.reg_infotitle);
        reg_info = activity.findViewById(R.id.reg_info);
        reg_nick = activity.findViewById(R.id.reg_nick);

        choose_skin_left.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            NvEventQueueActivity.getInstance().onRegisterSkinBackClick();
        });

        choose_skin_right.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            NvEventQueueActivity.getInstance().onRegisterSkinNextClick();
        });

        choose_skin_notf_close.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            Utils.HideLayout(choose_skin_notf_close, true);
            Utils.HideLayout(choose_skin_notf, true);
        });

        choose_skin_btn.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            NvEventQueueActivity.getInstance().onRegisterClick(reg_password.getText().toString(), MailString, choosesex);
        });

        choosesex_btn.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            if (choosesex > 0)
            {
                NvEventQueueActivity.getInstance().onRegisterChooseSkinClick(choosesex);
                Utils.ShowLayout(skin_layout, true);
                Utils.HideLayout(sex_layout, true);
            }
        });

        choosesex_male.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            choosesex = 1;
            choosesex_male.setBackgroundResource(R.drawable.choosesex_male_active);
            choosesex_female.setBackgroundResource(R.drawable.choosesex_female);
        });

        choosesex_female.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            choosesex = 2;
            choosesex_male.setBackgroundResource(R.drawable.choosesex_male);
            choosesex_female.setBackgroundResource(R.drawable.choosesex_female_active);
        });

        reg_play.setOnClickListener(view -> {
            //view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            if (reg_password.getText().length() > 5 && (reg_password.getText().toString().equals(reg_passwordtwo.getText().toString())))
            {
                Utils.ShowLayout(sex_layout, true);
                Utils.HideLayout(registration_layout, true);
            }
        });

        reg_back.setOnClickListener(view -> {
            Utils.HideLayout(reg_right2, false);
            Utils.ShowLayout(reg_right1, false);
            reg_skin.setBackgroundResource(R.drawable.reg_skin);
            reg_infotitle.setText("Выберите способ авторизации!");
            reg_info.setVisibility(view.GONE);
        });

        reg_btn.setOnClickListener(view -> {
            Utils.HideLayout(reg_right1, false);
            Utils.ShowLayout(reg_right2, false);
            reg_skin.setBackgroundResource(R.drawable.reg_skin1);
            reg_infotitle.setText("Для начала игры заполните все поля");
            reg_info.setVisibility(view.VISIBLE);
        });

        reg_password.addTextChangedListener(new TextWatcher() {
            @Override
            public void afterTextChanged(Editable s) {}

            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {}

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                if (reg_password.getText().length() > 5 && (reg_password.getText().toString().equals(reg_passwordtwo.getText().toString())))
                {
                    reg_play.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#fbc02d")));
                }
                else
                {
                    reg_play.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#004261")));
                }
            }
        });

        reg_passwordtwo.addTextChangedListener(new TextWatcher() {
            @Override
            public void afterTextChanged(Editable s) {}

            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {}

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                if (reg_password.getText().length() > 5 && (reg_password.getText().toString().equals(reg_passwordtwo.getText().toString())))
                {
                    reg_play.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#fbc02d")));
                }
                else
                {
                    reg_play.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#004261")));
                }
            }
        });

        reg_mail.addTextChangedListener(new TextWatcher() {
            @Override
            public void afterTextChanged(Editable s) {}

            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {}

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                if (reg_mail.getText().length() > 5)
                {
                    MailString = reg_mail.getText().toString();
                }
                else
                {
                    MailString = "";
                }
            }
        });

        Utils.HideLayout(br_registration_layout, false);
    }

    public void Show(String nick, int id) {
        choosesex = 0;
        String strnickid = String.format("%s [%d]", nick, id);
        reg_nick.setText(String.valueOf(strnickid));
        Utils.ShowLayout(registration_layout, false);
        Utils.ShowLayout(br_registration_layout, false);
    }

    public void Hide() {
        Utils.HideLayout(br_registration_layout, true);
    }
}