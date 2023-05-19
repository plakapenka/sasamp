package com.sasamp.cr.gui;

import android.app.Activity;
import android.graphics.drawable.Drawable;
import android.os.CountDownTimer;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.sasamp.cr.R;
import com.sasamp.cr.gui.util.Utils;

public class Notification {
    Activity activity;
    ConstraintLayout constraintLayout;
    FrameLayout br_not_bg;
    FrameLayout br_not_fl;
    View br_not_view;
    ImageView br_not_icon;
    FrameLayout br_title_text;
    TextView br_title_text_title;
    TextView br_title_text_text;
    TextView br_not_text;
    TextView br_not_text2;
    Button br_not_firstbutton;
    Button br_not_secondbutton;
    ProgressBar br_not_progress;

    // static //
    private int type, duration;
    // end static //

    static final int POSSITIVE = 1;
    static final int NEGATIVE = 0;

    native void sendClick(int actionId, int buttonId);
    native void init();
    CountDownTimer countDownTimer;

    public Notification (Activity activity) {
        init();

        this.activity = activity;
        constraintLayout = activity.findViewById(R.id.constraintLayout_notif);
        br_not_bg = this.activity.findViewById(R.id.br_not_bg);
        br_not_fl = this.activity.findViewById(R.id.br_not_fl);
        br_not_view = this.activity.findViewById(R.id.br_not_view);
        br_not_icon = this.activity.findViewById(R.id.br_not_icon);
        br_title_text = this.activity.findViewById(R.id.br_title_text);
        br_title_text_title = this.activity.findViewById(R.id.br_title_text_title);
        br_title_text_text = this.activity.findViewById(R.id.br_title_text_text);
        br_not_text = this.activity.findViewById(R.id.br_not_text);
        br_not_text2 = this.activity.findViewById(R.id.br_not_text2);
        br_not_firstbutton = this.activity.findViewById(R.id.br_not_firstbutton);
        br_not_secondbutton = this.activity.findViewById(R.id.br_not_secondbutton);
        br_not_progress = this.activity.findViewById(R.id.br_not_progress);
        Utils.HideLayout(constraintLayout, false);

        final GestureDetector gdt = new GestureDetector(new GestureListener());
        br_not_bg.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(final View view, final MotionEvent event) {
                gdt.onTouchEvent(event);
                return true;
            }
        });
    }

    public void ShowNotification (int type, String text, int duration, int actionId) {
        activity.runOnUiThread( () -> {
            Utils.HideLayout(constraintLayout, false);
            clearData();

            this.type = type;
            this.duration = duration;

            if (duration != 0) {
                br_not_progress.setMax(this.duration * 1000);
                br_not_progress.setProgress(this.duration * 1000);
            } else {
                br_not_progress.setMax(100);
                br_not_progress.setProgress(100);
            }

            switch (this.type) {
                case 0:
                    Drawable progressbar = activity.getResources().getDrawable(R.drawable.notify_progressbar_red);
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
                    Drawable progressbar1 = activity.getResources().getDrawable(R.drawable.notify_progressbar_green);
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
                    Drawable progressbar2 = activity.getResources().getDrawable(R.drawable.notify_progressbar_red);
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
                    Drawable progressbar3 = activity.getResources().getDrawable(R.drawable.notify_progressbar_green);
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
                    Drawable progressbar4 = activity.getResources().getDrawable(R.drawable.notify_progressbar_yellow);
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
                    Drawable progressbar5 = activity.getResources().getDrawable(R.drawable.notify_progressbar_yellow);
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
                    Drawable progressbar6 = activity.getResources().getDrawable(R.drawable.notify_progressbar_yellow);
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
                view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
                sendClick(actionId, POSSITIVE);
                hide(true);
            });

            br_not_secondbutton.setOnClickListener(view -> {
                view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
                sendClick(actionId, NEGATIVE);
                hide(true);
            });

            if (duration != 0) {
                startCountdown();
            }
            Utils.ShowLayout(constraintLayout, true);
        });
    }

    private void setMargins (View view, int left, int top, int right, int bottom) {
        if (view.getLayoutParams() instanceof ViewGroup.MarginLayoutParams) {
            ViewGroup.MarginLayoutParams p = (ViewGroup.MarginLayoutParams) view.getLayoutParams();
            p.setMargins(left, top, right, bottom);
            view.requestLayout();
        }
    }

    private void clearData() {
        this.type = -1;
        this.duration = -1;
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
                hide(true);
            }
        }.start();
    }
    public void hide(boolean right) {
        activity.runOnUiThread(() -> {
            if (constraintLayout.getVisibility() == View.VISIBLE) {
                if (countDownTimer != null) {
                    countDownTimer.cancel();
                    countDownTimer = null;
                }
                if (right) {
                    constraintLayout.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.popup_hide_notif_to_right));
                } else {
                    constraintLayout.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.popup_hide_notification));
                }

                constraintLayout.setVisibility(View.GONE);
            }
        });
    }
    private static final int SWIPE_MIN_DISTANCE = 120;
    private static final int SWIPE_THRESHOLD_VELOCITY = 200;

    private class GestureListener extends GestureDetector.SimpleOnGestureListener {
        @Override
        public boolean onSingleTapUp(MotionEvent e)
        {
            hide(false);
            return false;
        }
        @Override
        public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {

            if(e1.getX() - e2.getX() > SWIPE_MIN_DISTANCE && Math.abs(velocityX) > SWIPE_THRESHOLD_VELOCITY) {
                hide(false);
            }  else if (e2.getX() - e1.getX() > SWIPE_MIN_DISTANCE && Math.abs(velocityX) > SWIPE_THRESHOLD_VELOCITY) {
                hide(true);
            }


            return false;
        }
    }
}