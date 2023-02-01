package com.liverussia.cr.gui;

import android.app.Activity;
import android.view.SurfaceView;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;

import java.util.Timer;
import java.util.TimerTask;

public class ChooseServer {
    private ProgressBar loadscreen_progressBar;
    private TextView loadscreen_loadtext;
    private ConstraintLayout loadscreen_main_layout;
    private SurfaceView main_sv;
    private Activity activity;
    private Boolean isHided = false;

    public ChooseServer(Activity activity){
        this.activity = activity;

        main_sv = activity.findViewById(R.id.main_sv);
        loadscreen_main_layout = activity.findViewById(R.id.loadscreen_main_layout);
        loadscreen_progressBar = activity.findViewById(R.id.loadscreen_progressBar);
        loadscreen_loadtext = activity.findViewById(R.id.loadscreen_percent_text);
       // main_sv.setAlpha(0);
      //  Utils.HideLayout(serverLayout, false);
    }

    public void Update(int percent) {
        if (percent <= 100)
        {
            loadscreen_loadtext.setText(String.format("%d%%", Integer.valueOf(percent)));
            loadscreen_progressBar.setProgress(percent);
        }
        else {
            if(isHided)return;

            TimerTask task = new TimerTask() {
                public void run() {
                    activity.runOnUiThread(() -> {
                        Utils.HideLayout(loadscreen_main_layout, true);
                    });

                }
            };
            Timer timer = new Timer("Timer");

            timer.schedule(task, 900L);
            isHided = true;

//            loadscreen_main_layout.setVisibility(View.GONE);
          //  Utils.ShowLayout(main_sv, true);
//
        }
    }

}