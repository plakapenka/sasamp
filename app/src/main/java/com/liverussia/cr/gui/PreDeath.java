package com.liverussia.cr.gui;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.text.Html;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;

import java.util.Random;

public class PreDeath {
    private ConstraintLayout pre_death_layout;
    private Button toHospitalButton;
    private Button waitHelpButton;
    private TextView pre_death_killer_text;
    private int timeRemaining = 0;
    private Activity activity;
    private ConstraintLayout pre_death_deathbuttons_layout;
    private ConstraintLayout pde_death_game_buttons;
    private ConstraintLayout pre_death_adr_button;
    private ConstraintLayout pre_death_df_button;
    private TextView pre_death_pulse_count;
    private TextView pre_death_health_count;
    TextView pre_death_caption_textview;
    public native void medicPreDeathExit(int buttonID);
    public native void medicMiniGameExit(int typeId);
    private native void init();

    private int pulse = 0;
    private int health = 0;

    public PreDeath(Activity activity)
    {
        init();

        this.activity = activity;
        pre_death_caption_textview = activity.findViewById(R.id.pre_death_caption_textview);
        pre_death_adr_button = activity.findViewById(R.id.pre_death_adr_button);
        pde_death_game_buttons = activity.findViewById(R.id.pde_death_game_buttons);
        pre_death_deathbuttons_layout = activity.findViewById(R.id.pre_death_deathbuttons_layout);
        pre_death_layout = activity.findViewById(R.id.pre_death_layout);
        pre_death_killer_text = activity.findViewById(R.id.pre_death_killer_text);
        waitHelpButton = activity.findViewById(R.id.preDeath_wait_help_butt);
        toHospitalButton = activity.findViewById(R.id.preDeath_to_hospital_button);
        pre_death_pulse_count = activity.findViewById(R.id.pre_death_pulse_count);
        pre_death_health_count = activity.findViewById(R.id.pre_death_health_count);

        waitHelpButton.setOnClickListener(view -> {
            Utils.HideLayout(pre_death_layout, false);
            medicPreDeathExit(0);
        });
        toHospitalButton.setOnClickListener(view -> {
            if(timeRemaining > 0)return;
            Utils.HideLayout(pre_death_layout, false);
            medicPreDeathExit(1);
        });

        pre_death_adr_button.setOnClickListener(view -> {
            Random random = new Random();
            pulse += 5 + random.nextInt(5);
            if(pulse > 80) pulse = 70+random.nextInt(9);

            if(health >= 100 && pulse >= 70)
            {
                pre_death_layout.setVisibility(View.GONE);

                medicMiniGameExit(1);
                return;
            }
            updatePulseAndHealth();
        });

        pre_death_df_button = activity.findViewById(R.id.pre_death_df_button);
        pre_death_df_button.setOnClickListener(view -> {
            Random random = new Random();
            health += 10 + random.nextInt(5);
            if(health > 100) health = 100;

            if(health >= 100 && pulse >= 70)
            {
                pre_death_layout.setVisibility(View.GONE);

                medicMiniGameExit(1);
                return;
            }
            updatePulseAndHealth();
        });
        Utils.HideLayout(pre_death_layout, false);
    }
    void hide() {
        activity.runOnUiThread( () -> {
            pre_death_layout.setVisibility(View.GONE);
        });
    }
    public void setButtonActive()
    {
        activity.runOnUiThread(new Runnable() {
            public void run()  {
                toHospitalButton.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#ffffff")));
            }
        });
    }
    public void setButtonText(String str)
    {
        activity.runOnUiThread(new Runnable() {
            public void run()  {
                toHospitalButton.setText(str);
            }
        });
    }
    void showPreDeath(String killerName, int killerID)
    {// death
        activity.runOnUiThread(() -> {

            pre_death_caption_textview.setText(Html.fromHtml("Вы были <font color='#fbc02d'>ранены</font> игроком"));
            timeRemaining = 15;

            pde_death_game_buttons.setVisibility(View.GONE);
            pre_death_deathbuttons_layout.setVisibility(View.VISIBLE);

            pre_death_killer_text.setText(String.format("%s (%d)", killerName, killerID));

            Utils.ShowLayout(pre_death_layout, true);

            Thread thread = new Thread(runTimeRemaining);
            thread.start();
        });
    }
    void updatePulseAndHealth(){
        activity.runOnUiThread(() -> {
            pre_death_pulse_count.setText(String.format("%d", pulse));
            pre_death_health_count.setText(String.format("%d", health));
        });
    }
    void showMiniGame(String playerName){
        pulse = health = 0;
        activity.runOnUiThread(() -> {
            pre_death_caption_textview.setText(Html.fromHtml("<font color='#fbc02d'>Пострадавший</font>"));
            pde_death_game_buttons.setVisibility(View.VISIBLE);
            pre_death_deathbuttons_layout.setVisibility(View.GONE);
            pre_death_killer_text.setText(playerName);
            updatePulseAndHealth();

            Utils.ShowLayout(pre_death_layout, true);
        });
    }
    @SuppressLint("SetTextI18n")
    private Runnable runTimeRemaining = () -> {
        for(int i = 0; i < 20; i++)
        {
            if(Thread.currentThread().isInterrupted())
            {
                timeRemaining = 0;
                setButtonText("В БОЛЬНИЦУ");
                break;
            }
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
                timeRemaining = 0;
                setButtonText("В БОЛЬНИЦУ");
                break;
            }
        }
    };

    }
