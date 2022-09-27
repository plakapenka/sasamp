package com.liverussia.cr.gui;

import android.app.Activity;
import android.os.CountDownTimer;
import android.view.MotionEvent;
import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.nvidia.devtech.NvEventQueueActivity;

import org.w3c.dom.Text;

import java.util.Arrays;
import java.util.Collections;

public class SamwillManager {

    private FrameLayout br_samwill_layout;
    private ImageView samwill_1;
    private ImageView samwill_2;
    private ImageView samwill_3;
    private ImageView samwill_4;
    private ImageView samwill_5;
    private ImageView samwill_btn;

    public TextView samwill_procent;
    public ProgressBar samwill_progress;

    public CountDownTimer countDownTimer;

    int samwill1;
    int samwill2;
    int samwill3;
    int samwill4;
    int samwill5;
    int samwillpacket;
    Integer[] samwillarray = new Integer[25001];

    public SamwillManager(Activity activity){
        br_samwill_layout = activity.findViewById(R.id.br_samwill_layout);
        samwill_1 = activity.findViewById(R.id.samwill_1);
        samwill_2 = activity.findViewById(R.id.samwill_2);
        samwill_3 = activity.findViewById(R.id.samwill_3);
        samwill_4 = activity.findViewById(R.id.samwill_4);
        samwill_5 = activity.findViewById(R.id.samwill_5);
        samwill_btn = activity.findViewById(R.id.samwill_btn);
        samwill_procent = activity.findViewById(R.id.samwill_procent);
        samwill_progress = activity.findViewById(R.id.samwill_progress);

        for(int i = 0; i < 25001; i++) {
            samwillarray[i] = i;
        }
        Arrays.sort(samwillarray, Collections.reverseOrder());

        samwill_btn.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            if (samwill_progress.getProgress() > 2500 && samwill_progress.getProgress() < 4050)
            {
                samwillpacket++;
                samwill1 = 1;
                samwill_1.setImageResource(R.drawable.samwill_green);
            }
            if (samwill_progress.getProgress() > 7100 && samwill_progress.getProgress() < 8650)
            {
                samwillpacket++;
                samwill2 = 1;
                samwill_2.setImageResource(R.drawable.samwill_green);
            }
            if (samwill_progress.getProgress() > 11720 && samwill_progress.getProgress() < 13250)
            {
                samwillpacket++;
                samwill3 = 1;
                samwill_3.setImageResource(R.drawable.samwill_green);
            }
            if (samwill_progress.getProgress() > 16330 && samwill_progress.getProgress() < 17870)
            {
                samwillpacket++;
                samwill4 = 1;
                samwill_4.setImageResource(R.drawable.samwill_green);
            }
            if (samwill_progress.getProgress() > 20920 && samwill_progress.getProgress() < 22480)
            {
                samwillpacket++;
                samwill5 = 1;
                samwill_5.setImageResource(R.drawable.samwill_green);
            }
        });

        Utils.HideLayout(br_samwill_layout, false);
    }


    public void Show() {
        samwillpacket = 0;
        samwill1 = 0;
        samwill2 = 0;
        samwill3 = 0;
        samwill4 = 0;
        samwill5 = 0;
        samwill_1.setImageResource(R.drawable.samwill_gray);
        samwill_2.setImageResource(R.drawable.samwill_gray);
        samwill_3.setImageResource(R.drawable.samwill_gray);
        samwill_4.setImageResource(R.drawable.samwill_gray);
        samwill_5.setImageResource(R.drawable.samwill_gray);
        samwill_progress.setProgress(25000);
        Utils.ShowLayout(br_samwill_layout, true);
        startCountdown();
    }

    public void Hide() {
        NvEventQueueActivity.getInstance().onSamwillHideGame(samwillpacket);
        Utils.HideLayout(br_samwill_layout, true);
    }

    public void startCountdown() {
        if (countDownTimer != null) {
            countDownTimer.cancel();
            countDownTimer = null;
        }
        countDownTimer = new CountDownTimer(samwill_progress.getProgress(), 1) {
            @Override
            public void onTick(long j) {
                samwill_progress.setProgress(samwillarray[(int)j]);
                int progresstext = samwill_progress.getProgress() / 25 / 10;
                String str = String.format("%d", progresstext);
                samwill_procent.setText(String.valueOf(str) + "%");
                if (samwill_progress.getProgress() > 4050 && samwill1 != 1)
                {
                    samwill1 = 0;
                    samwill_1.setImageResource(R.drawable.samwill_red);
                }
                if (samwill_progress.getProgress() > 8650 && samwill2 != 1)
                {
                    samwill2 = 0;
                    samwill_2.setImageResource(R.drawable.samwill_red);
                }
                if (samwill_progress.getProgress() > 13250 && samwill3 != 1)
                {
                    samwill3 = 0;
                    samwill_3.setImageResource(R.drawable.samwill_red);
                }
                if (samwill_progress.getProgress() > 17870 && samwill4 != 1)
                {
                    samwill4 = 0;
                    samwill_4.setImageResource(R.drawable.samwill_red);
                }
                if (samwill_progress.getProgress() > 22480 && samwill5 != 1)
                {
                    samwill5 = 0;
                    samwill_5.setImageResource(R.drawable.samwill_red);
                }
            }
            @Override
            public void onFinish() {
                Hide();
            }
        }.start();
    }
}