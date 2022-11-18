package com.liverussia.cr.gui;

import static com.google.android.material.internal.ContextUtils.getActivity;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.core.content.ContextCompat;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.nvidia.devtech.NvEventQueueActivity;

public class PreDeath {
    private ConstraintLayout pre_death_layout;
    private Button toHospitalButton;
    private Button waitHelpButton;
    private TextView pre_death_killer_text;
    private int timeRemaining = 0;
    private Activity main_activity;
    public native void OnClickPreDeathButton(int buttonID);

    public PreDeath(Activity activity)
    {
        main_activity = activity;
        pre_death_layout = activity.findViewById(R.id.pre_death_layout);
        pre_death_killer_text = activity.findViewById(R.id.pre_death_killer_text);
        waitHelpButton = activity.findViewById(R.id.preDeath_wait_help_butt);
        toHospitalButton = activity.findViewById(R.id.preDeath_to_hospital_button);

        waitHelpButton.setOnClickListener(view -> {
            Utils.HideLayout(pre_death_layout, false);
            OnClickPreDeathButton(0);
        });
        toHospitalButton.setOnClickListener(view -> {
            Utils.HideLayout(pre_death_layout, false);
            OnClickPreDeathButton(1);
        });

        Utils.HideLayout(pre_death_layout, false);
    }
    public void setButtonActive()
    {
        main_activity.runOnUiThread(new Runnable() {
            public void run()  {
                toHospitalButton.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#ffffff")));
            }
        });
    }
    public void setButtonText(String str)
    {
        main_activity.runOnUiThread(new Runnable() {
            public void run()  {
                toHospitalButton.setText(str);
            }
        });
    }
    public void Show(String killerName, int killerID)
    {
        timeRemaining = 15;

        String _str = String.format("%s (%d)", killerName, killerID);
        pre_death_killer_text.setText(_str);

        Utils.ShowLayout(pre_death_layout, true);

        Thread thread = new Thread(runTimeRemaining);
        thread.start();
    }

    @SuppressLint("SetTextI18n")
    private Runnable runTimeRemaining = () -> {
        for(int i = 0; i < 20; i++)
        {
            timeRemaining --;
            if(timeRemaining == 0)
            {
                //toHospitalButton.setBackgroundTintList(ContextCompat.getColor(this));
                setButtonText("В БОЛЬНИЦУ");
                break;
            }
            else
            {

                setButtonText("Доступно через "+timeRemaining);

            }
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    };

    }
