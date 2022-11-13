package com.liverussia.cr.gui;

import android.app.Activity;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.text.Html;
import android.view.View;
import android.widget.*;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.liverussia.cr.gui.util.SeekArc;
import com.nvidia.devtech.NvEventQueueActivity;

import java.util.Formatter;

public class Speedometer {
    private boolean blinker;
    private ConstraintLayout spedometr_layout;
    private Activity activity;
    private TextView mCarHP;
    private ImageView mEngine;
    private ImageView mBelt;
    private TextView mFuel;
    private ConstraintLayout mInputLayout;
    private ImageView mLight;
    private ImageView mLock;
    private TextView mMileage;
    private TextView mSpeed;
    private SeekArc mSpeedLine;
    private ImageView turn_right;
    private ImageView turn_left;
    boolean[] turn_stat = new boolean[3];
    private static final int TURN_SIGNAL_LEFT = 0;
    private static final int TURN_SIGNAL_ALL = 1;
    private static final int TURN_SIGNAL_RIGHT = 2;

    native void ClickSpedometr(int turnID, boolean toggle);

    public Speedometer(Activity activity){
        ConstraintLayout relativeLayout = activity.findViewById(R.id.speedometer);
        spedometr_layout = activity.findViewById(R.id.spedometr_layout);
        mInputLayout = relativeLayout;
        mSpeed = activity.findViewById(R.id.speed_text);
        mFuel = activity.findViewById(R.id.speed_fuel_text);
        mCarHP = activity.findViewById(R.id.speed_car_hp_text);
        mMileage = activity.findViewById(R.id.textView2);
        mSpeedLine = activity.findViewById(R.id.speed_line);
        mEngine = activity.findViewById(R.id.imageView12);
        mLight = activity.findViewById(R.id.imageView9);
        mBelt = activity.findViewById(R.id.speed_engine_ico);
        mLock = activity.findViewById(R.id.speed_lock_ico);
        turn_right = activity.findViewById(R.id.turn_right);
        turn_left = activity.findViewById(R.id.turn_left);

        turn_left.setOnClickListener(view -> {
            turn_stat[TURN_SIGNAL_LEFT] = !turn_stat[TURN_SIGNAL_LEFT];
            ClickSpedometr(TURN_SIGNAL_LEFT, turn_stat[TURN_SIGNAL_LEFT]);
        });
        turn_right.setOnClickListener(view -> {
            turn_stat[TURN_SIGNAL_RIGHT] = !turn_stat[TURN_SIGNAL_RIGHT];
            ClickSpedometr(TURN_SIGNAL_RIGHT, turn_stat[TURN_SIGNAL_RIGHT]);
        });
        spedometr_layout.setOnClickListener(view -> {
            turn_stat[TURN_SIGNAL_ALL] = !turn_stat[TURN_SIGNAL_ALL];
            ClickSpedometr(TURN_SIGNAL_ALL, turn_stat[TURN_SIGNAL_ALL]);
        });
        mEngine.setOnClickListener(view ->  {
            NvEventQueueActivity.getInstance().onSpeedEngineClick();
        });

        mLight.setOnClickListener(view ->  {
            NvEventQueueActivity.getInstance().onSpeedLightsClick();
        });

        Utils.HideLayout(relativeLayout, false);
    }

    public void UpdateSpeedInfo(int speed, int fuel, int hp, int mileage, int engine, int light, int belt, int lock){
        hp= (int) hp/10;
        mFuel.setText(new Formatter().format("%03d", Integer.valueOf(fuel)).toString());
        mMileage.setText(new Formatter().format("%06d", Integer.valueOf(mileage)).toString());
        mCarHP.setText(new Formatter().format("%d%s", Integer.valueOf(hp), "%").toString());
        mSpeedLine.setProgress(speed);
        mSpeed.setText(String.valueOf(speed));

        mEngine.setImageResource(engine == 1 ? R.drawable.speedometr_engine_on : R.drawable.speedometr_engine_off);
        mLight.setImageResource(light == 2 ? R.drawable.speedometr_light_on : R.drawable.speedometr_light_off);
        mBelt.setColorFilter(belt == 1 ? Color.parseColor("#FF0000") : Color.parseColor("#00FF00"), PorterDuff.Mode.SRC_IN);
        mLock.setColorFilter(lock == 1 ? Color.parseColor("#FF0000") : Color.parseColor("#00FF00"), PorterDuff.Mode.SRC_IN);


    }

    public void ShowSpeed() {
        Utils.ShowLayout(mInputLayout, false);
    }

    public void HideSpeed() {
        Utils.HideLayout(mInputLayout, false);
    }
}