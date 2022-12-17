package com.liverussia.cr.gui;

import android.animation.Animator;
import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.graphics.drawable.Drawable;
import android.text.Html;
import android.view.LayoutInflater;
import android.view.SurfaceView;
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
import androidx.core.content.res.ResourcesCompat;

import com.akexorcist.roundcornerprogressbar.RoundCornerProgressBar;
import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.liverussia.launcher.async.dto.response.Servers;
import com.liverussia.launcher.utils.MainUtils;
import com.nvidia.devtech.NvEventQueueActivity;

import java.io.UnsupportedEncodingException;
import java.util.List;
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
        loadscreen_loadtext = activity.findViewById(R.id.loadscreen_loadtext);
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