package com.liverussia.cr.gui;

import android.animation.Animator;
import android.app.Activity;
import android.graphics.drawable.Drawable;
import android.os.CountDownTimer;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.core.content.ContextCompat;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.nvidia.devtech.NvEventQueueActivity;

import java.io.UnsupportedEncodingException;

public class Notification {
    private Activity aactivity;
    private ConstraintLayout constraintLayout;
    private FrameLayout br_not_bg;
    private FrameLayout br_not_fl;
    private View br_not_view;
    private ImageView br_not_icon;
    private FrameLayout br_title_text;
    private TextView br_title_text_title;
    private TextView br_title_text_text;
    private TextView br_not_text;
    private TextView br_not_text2;
    private Button br_not_firstbutton;
    private Button br_not_secondbutton;
    private ProgressBar br_not_progress;

    // static //
    private int type, duration;
    private String text, actionforBtn, textBtn;
    // end static //

    private CountDownTimer countDownTimer;

    public Notification (Activity activity) {
        aactivity = activity;
        constraintLayout = activity.findViewById(R.id.constraintLayout_notif);
        br_not_bg = aactivity.findViewById(R.id.br_not_bg);
        br_not_fl = aactivity.findViewById(R.id.br_not_fl);
        br_not_view = aactivity.findViewById(R.id.br_not_view);
        br_not_icon = aactivity.findViewById(R.id.br_not_icon);
        br_title_text = aactivity.findViewById(R.id.br_title_text);
        br_title_text_title = aactivity.findViewById(R.id.br_title_text_title);
        br_title_text_text = aactivity.findViewById(R.id.br_title_text_text);
        br_not_text = aactivity.findViewById(R.id.br_not_text);
        br_not_text2 = aactivity.findViewById(R.id.br_not_text2);
        br_not_firstbutton = aactivity.findViewById(R.id.br_not_firstbutton);
        br_not_secondbutton = aactivity.findViewById(R.id.br_not_secondbutton);
        br_not_progress = aactivity.findViewById(R.id.br_not_progress);
        Utils.HideLayout(constraintLayout, false);
    }

    public void ShowNotification (int type, String text, int duration, String actionforBtn, String textBtn, int actionId) {
        Utils.HideLayout(constraintLayout, false);
        clearData();

        this.type = type;
        this.text = text;
        this.duration = duration;
        this.actionforBtn = actionforBtn;
        this.textBtn = textBtn;

        if (duration != 0)
        {
            br_not_progress.setMax(this.duration * 1000);
            br_not_progress.setProgress(this.duration * 1000);
        }
        else
        {
            br_not_progress.setMax(100);
            br_not_progress.setProgress(100);
        }

        switch (this.type) {
            case 0:
                Drawable progressbar = aactivity.getResources().getDrawable(R.drawable.notify_progressbar_red);
                br_not_bg.setBackgroundResource(R.drawable.notify_background_red);
                br_not_progress.setProgressDrawable(progressbar);
                br_not_firstbutton.setVisibility(View.GONE);
                br_not_secondbutton.setVisibility(View.GONE);
                br_not_fl.setVisibility(View.VISIBLE);
                setMargins(br_title_text, 0, 2, 25, 12);
                br_title_text.setVisibility(View.GONE);
                br_not_text2.setVisibility(View.VISIBLE);
                br_not_text.setVisibility(View.GONE);
                br_not_view.setBackgroundResource(R.drawable.notify_error_bg);
                br_not_icon.setImageResource(R.drawable.notify_error);
                br_not_text2.setText(text);
                break;
            case 1:
                Drawable progressbar1 = aactivity.getResources().getDrawable(R.drawable.notify_progressbar_green);
                br_not_bg.setBackgroundResource(R.drawable.notify_background_green);
                br_not_progress.setProgressDrawable(progressbar1);
                br_not_firstbutton.setVisibility(View.GONE);
                br_not_secondbutton.setVisibility(View.GONE);
                br_not_fl.setVisibility(View.VISIBLE);
                setMargins(br_not_text, 0, 2, 25, 12);
                br_title_text.setVisibility(View.GONE);
                br_not_text2.setVisibility(View.GONE);
                br_not_text.setVisibility(View.VISIBLE);
                br_not_view.setBackgroundResource(R.drawable.notify_ruble_bg);
                br_not_icon.setImageResource(R.drawable.notify_ruble);
                br_not_text.setText(text);
                break;
            case 2:
                Drawable progressbar2 = aactivity.getResources().getDrawable(R.drawable.notify_progressbar_red);
                br_not_bg.setBackgroundResource(R.drawable.notify_background_red);
                br_not_progress.setProgressDrawable(progressbar2);
                br_not_firstbutton.setVisibility(View.GONE);
                br_not_secondbutton.setVisibility(View.GONE);
                br_not_fl.setVisibility(View.VISIBLE);
                setMargins(br_not_text, 0, 2, 25, 12);
                br_title_text.setVisibility(View.GONE);
                br_not_text2.setVisibility(View.GONE);
                br_not_text.setVisibility(View.VISIBLE);
                br_not_view.setBackgroundResource(R.drawable.notify_ruble_bg);
                br_not_icon.setImageResource(R.drawable.notify_ruble);
                br_not_text.setText(text);
                break;
            case 3:
                Drawable progressbar3 = aactivity.getResources().getDrawable(R.drawable.notify_progressbar_green);
                br_not_bg.setBackgroundResource(R.drawable.notify_background_green);
                br_not_progress.setProgressDrawable(progressbar3);
                br_not_firstbutton.setVisibility(View.GONE);
                br_not_secondbutton.setVisibility(View.GONE);
                br_not_fl.setVisibility(View.VISIBLE);
                setMargins(br_title_text, 0, 2, 25, 12);
                br_title_text.setVisibility(View.GONE);
                br_not_text2.setVisibility(View.VISIBLE);
                br_not_text.setVisibility(View.GONE);
                br_not_view.setBackgroundResource(R.drawable.notify_success_bg);
                br_not_icon.setImageResource(R.drawable.notify_success);
                br_not_text2.setText(text);
                break;
            case 4:
                Drawable progressbar4 = aactivity.getResources().getDrawable(R.drawable.notify_progressbar_yellow);
                br_not_bg.setBackgroundResource(R.drawable.notify_background_blue);
                br_not_progress.setProgressDrawable(progressbar4);
                br_not_firstbutton.setVisibility(View.VISIBLE);
                br_not_secondbutton.setVisibility(View.GONE);
                br_not_fl.setVisibility(View.GONE);
                setMargins(br_not_text, 25, 2, 25, 12);
                br_title_text.setVisibility(View.GONE);
                br_not_text2.setVisibility(View.GONE);
                br_not_text.setVisibility(View.VISIBLE);
                br_not_text.setText(text);
                br_not_firstbutton.setText(">>");
                break;
            case 5:
                Drawable progressbar5 = aactivity.getResources().getDrawable(R.drawable.notify_progressbar_yellow);
                br_not_bg.setBackgroundResource(R.drawable.notify_background_blue);
                br_not_progress.setProgressDrawable(progressbar5);
                br_not_firstbutton.setVisibility(View.VISIBLE);
                br_not_secondbutton.setVisibility(View.GONE);
                br_not_fl.setVisibility(View.GONE);
                setMargins(br_title_text, 25, 2, 25, 12);
                br_title_text.setVisibility(View.VISIBLE);
                br_not_text2.setVisibility(View.GONE);
                br_not_text.setVisibility(View.GONE);
                br_title_text_title.setText(text);
                br_title_text_text.setText("Нажмите, чтобы войти");
                br_not_firstbutton.setText("Войти");
                break;
            case 6:
                Drawable progressbar6 = aactivity.getResources().getDrawable(R.drawable.notify_progressbar_yellow);
                br_not_bg.setBackgroundResource(R.drawable.notify_background_blue);
                br_not_progress.setProgressDrawable(progressbar6);
                br_not_firstbutton.setVisibility(View.VISIBLE);
                br_not_secondbutton.setVisibility(View.VISIBLE);
                br_not_fl.setVisibility(View.GONE);
                setMargins(br_title_text, 25, 2, 25, 12);
                br_title_text.setVisibility(View.VISIBLE);
                br_not_text2.setVisibility(View.GONE);
                br_not_text.setVisibility(View.GONE);
                br_title_text_title.setText("Поступило предложение");
                br_title_text_text.setText(text);
                br_not_firstbutton.setText("Принять");
                br_not_secondbutton.setText("Отказать");
        }

        br_not_firstbutton.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(aactivity, R.anim.button_click));
            NvEventQueueActivity.getInstance().onNotifyFirstClick(actionId);
            HideNotification();
        });

        br_not_secondbutton.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(aactivity, R.anim.button_click));
            NvEventQueueActivity.getInstance().onNotifySecondClick(actionId);
            HideNotification();
        });

        if (duration != 0)
        {
            startCountdown();
        }
        Utils.ShowLayout(constraintLayout, true);
    }

    private void setMargins (View view, int left, int top, int right, int bottom) {
        if (view.getLayoutParams() instanceof ViewGroup.MarginLayoutParams) {
            ViewGroup.MarginLayoutParams p = (ViewGroup.MarginLayoutParams) view.getLayoutParams();
            p.setMargins(left, top, right, bottom);
            view.requestLayout();
        }
    }

    private void clearData() {
        this.text = "";
        this.type = -1;
        this.duration = -1;
        this.actionforBtn = "";
        this.textBtn = "";
    }

    public void startCountdown() {
        if (countDownTimer != null) {
            countDownTimer.cancel();
            countDownTimer = null;
        }
        countDownTimer = new CountDownTimer(br_not_progress.getProgress(), 1) {
            @Override
            public void onTick(long j) {
                br_not_progress.setProgress((int) j);
            }
            @Override
            public void onFinish() {
                HideNotification();
            }
        }.start();
    }
    public void HideNotification () {
        if (constraintLayout.getVisibility() == View.VISIBLE) {
            if (countDownTimer != null) {
                countDownTimer.cancel();
                countDownTimer = null;
            }
            constraintLayout.startAnimation(AnimationUtils.loadAnimation(aactivity, R.anim.popup_hide_notification));
            constraintLayout.setVisibility(View.GONE);
        }
    }
}