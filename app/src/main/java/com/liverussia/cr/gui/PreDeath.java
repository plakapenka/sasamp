package com.liverussia.cr.gui;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.TextView;

import androidx.core.content.ContextCompat;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.nvidia.devtech.NvEventQueueActivity;

public class PreDeath {
    private FrameLayout pre_death_layout;
    private Button toHospitalButton;
    private Button waitHelpButton;
    private TextView pre_death_killer_text;
    private int timeRemaining = 0;

    public PreDeath(Activity activity)
    {
        pre_death_layout = activity.findViewById(R.id.pre_death_layout);
        pre_death_killer_text = activity.findViewById(R.id.pre_death_killer_text);
        waitHelpButton = activity.findViewById(R.id.preDeath_wait_help_butt);
        toHospitalButton = activity.findViewById(R.id.preDeath_to_hospital_button);

        Utils.HideLayout(pre_death_layout, false);
    }

    public void Show(String killerName, int killerID)
    {
        Log.d("gg", "gavno = "+killerName);
        timeRemaining = 15;
        String _str = String.format("%s (%d)", killerName, killerID);
        //setContentView(R.layout.pre_death);
        //setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        Utils.ShowLayout(pre_death_layout, true);
     //   pre_death_killer_text.setText(_str);




    }

    @SuppressLint("SetTextI18n")
    private Runnable runTimeRemaining = () -> {
        for(int i = 0; i < 20; i++)
        {
            timeRemaining --;
            if(timeRemaining == 0)
            {
                              
                //toHospitalButton.setBackgroundTintList(ContextCompat.getColor(this));
                toHospitalButton.setText("В БОЛЬНИЦУ");
                break;
            }
            else
            {
                toHospitalButton.setText("Доступно через " + timeRemaining);

            }
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    };

    }
