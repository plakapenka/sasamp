package com.liverussia.cr.gui;

import android.app.Activity;
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

    float olistate, oliwaterstate, oiloilstate;

    Runnable runnableoilgame = new Runnable() {
        @Override
        public void run() {
            while(olistate<2000) {
                oliwaterstate -= 0.0001;
                oiloilstate -= 0.0001;
                if (oliwaterstate < 0)
                {
                    oliwaterstate = 0;
                }
                if (oliwaterstate > 1000)
                {
                    oliwaterstate = 1000;
                }
                if (oiloilstate < 0)
                {
                    oiloilstate = 0;
                }
                if (oiloilstate > 1000)
                {
                    oiloilstate = 1000;
                }
                oil_water_progress.setProgress((int)oliwaterstate);
                oil_oil_progress.setProgress((int)oiloilstate);
                olistate = oliwaterstate + oiloilstate;
            }
            Hide();
        }
    };

    public OilFactoryManager(Activity activity){
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
            oil_water_progress.setProgress((int)oliwaterstate);
            String stroilwaterprocent = String.format("%d%%", (int)oliwaterstate/10);
            oil_water_procent.setText(String.valueOf(stroilwaterprocent));
        });

        oil_oil_btn.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            oiloilstate += 100;
            oil_oil_progress.setProgress((int)oiloilstate);
            String stroiloilprocent = String.format("%d%%", (int)oiloilstate/10);
            oil_oil_procent.setText(String.valueOf(stroiloilprocent));
        });

        Utils.HideLayout(br_oilfactory_layout, false);
    }

    public void Show() {
        olistate = 0;
        oliwaterstate = 0;
        oiloilstate = 0;
        Utils.ShowLayout(br_oilfactory_layout, true);
        Thread thread = new Thread(runnableoilgame);
        thread.start();
    }

    public void Hide() {
        NvEventQueueActivity.getInstance().onOilFactoryGameClose();
        Utils.HideLayout(br_oilfactory_layout, true);
    }
}