package com.liverussia.cr.gui;

import android.app.Activity;
import android.util.Log;
import android.view.SurfaceView;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.skydoves.progressview.ProgressView;

import java.util.Timer;
import java.util.TimerTask;

public class InGameLoadingScreen {
    ProgressView loadscreen_progressBar;
    ConstraintLayout loadscreen_main_layout;
    SurfaceView main_sv;
    Activity activity;
    Boolean isHided = false;

    public InGameLoadingScreen(Activity activity){
        this.activity = activity;
        activity.runOnUiThread(()-> {
            main_sv = activity.findViewById(R.id.main_sv);
            loadscreen_main_layout = activity.findViewById(R.id.loadscreen_main_layout);
            loadscreen_progressBar = activity.findViewById(R.id.loadscreen_progressBar);
        });
    }

    void updatePercent(int percent) {
        activity.runOnUiThread(()-> {
            loadscreen_progressBar.setProgress(percent);
        });

    }

    void hide()
    {
        TimerTask task = new TimerTask() {
            public void run() {
                activity.runOnUiThread(() -> {
                    Utils.HideLayout(loadscreen_main_layout, true);
                });

            }
        };
        Timer timer = new Timer("Timer");

        timer.schedule(task, 900L);
    }

}