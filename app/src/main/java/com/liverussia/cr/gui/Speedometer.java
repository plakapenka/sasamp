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

    public Speedometer(Activity activity){
        ConstraintLayout relativeLayout = activity.findViewById(R.id.speedometer);
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
        mLight.setImageResource(light == 1 ? R.drawable.speedometr_light_on : R.drawable.speedometr_light_off);
        mBelt.setColorFilter(belt == 1 ? Color.parseColor("#FF0000") : Color.parseColor("#00FF00"), PorterDuff.Mode.SRC_IN);
        mLock.setColorFilter(lock == 1 ? Color.parseColor("#FF0000") : Color.parseColor("#00FF00"), PorterDuff.Mode.SRC_IN);

        mEngine.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                NvEventQueueActivity.getInstance().onSpeedEngineClick();
            }
        });

        mLight.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                NvEventQueueActivity.getInstance().onSpeedLightsClick();
            }
        });
    }

    public void ShowSpeed() {
        Utils.ShowLayout(mInputLayout, false);
    }

    public void HideSpeed() {
        Utils.HideLayout(mInputLayout, false);
    }
}