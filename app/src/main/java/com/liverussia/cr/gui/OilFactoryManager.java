package com.liverussia.cr.gui;

import android.app.Activity;
import android.os.CountDownTimer;
import android.util.Log;
import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.nvidia.devtech.NvEventQueueActivity;

public class OilFactoryManager {

    private ConstraintLayout br_oilfactory_layout;
    private ProgressBar oil_water_progress;
    private ProgressBar oil_oil_progress;
    private ImageView oil_water_btn;
    private ImageView oil_oil_btn;
    private TextView oil_water_procent;
    private TextView oil_oil_procent;
    private View oil_factory_exit_button;
    public CountDownTimer countDownTimer;

    int oliwaterstate, oiloilstate;

    public void startCountdown() {
        if (countDownTimer != null) {
            countDownTimer.cancel();
            countDownTimer = null;
        }
        countDownTimer = new CountDownTimer(999999999, 10) {
            @Override
            public void onTick(long j) {
                if(oiloilstate >= 998 && oliwaterstate >= 998){
                    countDownTimer.cancel();
                    countDownTimer.onFinish(); // сам не срабатывает при cancel :c
                    return;
                }
                oliwaterstate --;
                oiloilstate --;

                if (oliwaterstate < 0)      oliwaterstate = 0;
                if (oliwaterstate > 1000)    oliwaterstate = 1000;
                if (oiloilstate < 0)        oiloilstate = 0;
                if (oiloilstate > 1000)      oiloilstate = 1000;

                UpdateWater(oliwaterstate);
                UpdateOil(oiloilstate);
            }
            @Override
            public void onFinish() {
                if(oiloilstate >= 998 && oliwaterstate >= 998){
                    Hide(true);
                }else{
                    Hide(false);
                }
            }
        }.start();
    }
    public void UpdateWater(int percent){
        String stroilwaterproc = String.format("%d%%", percent / 10);
        oil_water_procent.setText(stroilwaterproc);
        oil_water_progress.setProgress(percent);
    }
    public void UpdateOil(int percent) {
        String stroiloilproc = String.format("%d%%", percent / 10);
        oil_oil_procent.setText(stroiloilproc);
        oil_oil_progress.setProgress(percent);
    }
    public OilFactoryManager(Activity activity){
        oil_factory_exit_button = activity.findViewById(R.id.oil_factory_exit_button);
        oil_factory_exit_button.setOnClickListener(view -> {
            countDownTimer.cancel();
            countDownTimer.onFinish();
        });
        br_oilfactory_layout = activity.findViewById(R.id.br_oilfactory_layout);
        oil_water_progress = activity.findViewById(R.id.oil_water_progress);
        oil_oil_progress = activity.findViewById(R.id.oil_oil_progress);
        oil_water_btn = activity.findViewById(R.id.oil_water_btn);
        oil_oil_btn = activity.findViewById(R.id.oil_oil_btn);
        oil_water_procent = activity.findViewById(R.id.oil_water_procent);
        oil_oil_procent = activity.findViewById(R.id.oil_oil_procent);

        oil_water_btn.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            oliwaterstate += 100;
            UpdateWater(oliwaterstate);
        });

        oil_oil_btn.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            oiloilstate += 100;
            UpdateOil(oiloilstate);
        });

        Utils.HideLayout(br_oilfactory_layout, false);
    }

    public void Show() {
        oliwaterstate = 0;
        oiloilstate = 0;
        Utils.ShowLayout(br_oilfactory_layout, true);
        startCountdown();
    }

    public void Hide(boolean succes) {
        NvEventQueueActivity.getInstance().onOilFactoryGameClose(succes);
        Utils.HideLayout(br_oilfactory_layout, true);
    }
}