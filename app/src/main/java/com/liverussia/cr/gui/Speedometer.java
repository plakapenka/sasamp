package com.liverussia.cr.gui;

import static java.lang.Thread.sleep;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.text.Html;
import android.util.Log;
import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.*;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;
import com.liverussia.cr.core.Samp;
import com.liverussia.cr.gui.util.Utils;
import com.liverussia.cr.gui.util.SeekArc;
import com.nvidia.devtech.NvEventQueueActivity;

import java.util.Formatter;
import java.util.Timer;
import java.util.TimerTask;

public class Speedometer {
    Activity activity;

    private boolean blinker;
    ConstraintLayout speedometer_main_layout;
    TextView speedometr_speed_text;
    ImageView speedometr_engine_icon;
    ImageView speedometr_light_icon;
    TextView speedometr_milliage_text;
    TextView speedometr_fuel_text;
    TextView speedometr_hp_text;
    ImageView speed_lock_ico;
    ImageView speedometr_blinker_icon;

    Thread timer_turnlight_left = null;
    Thread timer_turnlight_right = null;
    Thread timer_turnlight_all = null;

    boolean turnlight_left_state;
    boolean turnlight_right_state;
    boolean turnlight_all_state;

    private ImageView mBelt;
    private SeekArc mSpeedLine;
    ImageView speedometr_turn_right;
    ImageView speedometr_turn_left;
    boolean[] turn_stat = new boolean[3];

    static final int BUTTON_ENGINE = 0;
    static final int BUTTON_LIGHT = 1;
    static final int BUTTON_TURN_LEFT = 2;
    static final int BUTTON_TURN_RIGHT = 3;
    static final int BUTTON_TURN_ALL = 4;

    static final int TURN_SIGNAL_LEFT = 1;
    static final int TURN_SIGNAL_RIGHT = 2;
    static final int TURN_SIGNAL_ALL = 3;

    int turnlight_tick_sound_1;
    int turnlight_tick_sound_2;

    native void ClickSpedometr(int turnID, boolean toggle);
    native void sendClick(int clickId);

    public Speedometer(Activity activity)
    {
        this.activity = activity;

        activity.runOnUiThread(() -> {
            speedometr_blinker_icon = activity.findViewById(R.id.speedometr_blinker_icon);
            speedometr_speed_text = activity.findViewById(R.id.speedometr_speed_text);
            speedometr_fuel_text = activity.findViewById(R.id.speedometr_fuel_text);
            speedometr_hp_text = activity.findViewById(R.id.speedometr_hp_text);
            speedometr_milliage_text = activity.findViewById(R.id.speedometr_milliage_text);
            mSpeedLine = activity.findViewById(R.id.speed_line);
            speedometr_engine_icon = activity.findViewById(R.id.speedometr_engine_icon);
            speedometr_light_icon = activity.findViewById(R.id.speedometr_light_icon);
            speed_lock_ico = activity.findViewById(R.id.speed_lock_ico);
            speedometr_turn_right = activity.findViewById(R.id.speedometr_turn_right);
            speedometr_turn_left = activity.findViewById(R.id.speedometr_turn_left);

            turnlight_tick_sound_1 = Samp.soundPool.load(NvEventQueueActivity.getInstance(), R.raw.turnlight_tick_1, 0);
            turnlight_tick_sound_2 = Samp.soundPool.load(NvEventQueueActivity.getInstance(), R.raw.turnlight_tick_2, 0);

            speedometr_blinker_icon.setOnClickListener(view -> {
                sendClick(BUTTON_TURN_ALL);
            });

            speedometr_turn_left.setOnClickListener(view -> {
                view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
                sendClick(BUTTON_TURN_LEFT);
            });
            speedometr_turn_right.setOnClickListener(view -> {
                view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
                sendClick(BUTTON_TURN_RIGHT);
            });

            speedometr_engine_icon.setOnClickListener(view -> {
                view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
                sendClick(BUTTON_ENGINE);
            });

            speedometr_light_icon.setOnClickListener(view -> {
                view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
                sendClick(BUTTON_LIGHT);
            });


            speedometer_main_layout = activity.findViewById(R.id.speedometer_main_layout);
            speedometer_main_layout.setVisibility(View.VISIBLE);
            //   Utils.HideLayout(relativeLayout, false);
        });

    }

    void deleteThreads()
    {
        if(timer_turnlight_left != null && timer_turnlight_left.isAlive())
            timer_turnlight_left.interrupt();

        if(timer_turnlight_right != null && timer_turnlight_right.isAlive())
            timer_turnlight_right.interrupt();

        if(timer_turnlight_all != null && timer_turnlight_all.isAlive())
            timer_turnlight_all.interrupt();
    }

    void destructor()
    {
        activity.runOnUiThread( () -> {
            speedometer_main_layout.setVisibility(View.GONE);
            deleteThreads();
        });
    }

    @SuppressLint("DefaultLocale")
    void update(int speed, int fuel, int hp, int mileage, int engine, int light, int lock, int turnlight)
    {
        activity.runOnUiThread( () -> {

            speedometr_fuel_text.setText( String.format("%03d", fuel) );
            speedometr_milliage_text.setText( String.format("%06d", mileage) );
            speedometr_hp_text.setText( String.format("%d%%", hp / 10 ) );
            if(speed > 0 && engine == 1) {
                mSpeedLine.setProgress(speed);
                speedometr_speed_text.setText( String.valueOf(speed) );
            }
            else {
                mSpeedLine.setProgress(0);
                speedometr_speed_text.setText("0");
            }

            speedometr_engine_icon.setImageTintList(engine == 1 ? ColorStateList.valueOf( Color.parseColor("#01FE48") ) : null);
            speedometr_light_icon.setImageTintList(light == 1 ? ColorStateList.valueOf( Color.parseColor("#FAFF00") ): null);
          //  speedometr_engine_icon.setImageResource(engine == 1 ? R.drawable.speedometr_engine_on_icon : R.drawable.speedometr_engine_off_icon);
          //  speedometr_light_icon.setImageResource(light == 1 ? R.drawable.speedometr_light_on_icon : R.drawable.speedometr_light_off_icon);
            //  mBelt.setColorFilter(belt == 1 ? Color.parseColor("#FF0000") : Color.parseColor("#00FF00"), PorterDuff.Mode.SRC_IN);
            speed_lock_ico.setColorFilter(lock == 1 ? Color.parseColor("#FF0000") : Color.parseColor("#00FF00"), PorterDuff.Mode.SRC_IN);

            switch (turnlight)
            {
                case TURN_SIGNAL_LEFT: {
                    if(timer_turnlight_left == null || !timer_turnlight_left.isAlive()) {
                        deleteThreads();
                        timer_turnlight_left = new Thread(runnable_turnlight_left);
                        timer_turnlight_left.start();
                    }
                    break;
                }
                case TURN_SIGNAL_RIGHT: {
                    if(timer_turnlight_right == null || !timer_turnlight_right.isAlive()) {
                        deleteThreads();
                        timer_turnlight_right = new Thread(runnable_turnlight_right);
                        timer_turnlight_right.start();
                    }
                    break;
                }
                case TURN_SIGNAL_ALL: {
                    if(timer_turnlight_all == null || !timer_turnlight_all.isAlive()) {
                        deleteThreads();
                        timer_turnlight_all = new Thread(runnable_turnlight_all);
                        timer_turnlight_all.start();
                    }
                    break;
                }
                default: {
                    deleteThreads();
                    speedometr_blinker_icon.setImageTintList( null );
                    speedometr_turn_left.setImageResource(R.drawable.speedometr_turn_off);
                    speedometr_turn_right.setImageResource(R.drawable.speedometr_turn_off);
                }
            }

        });

    }

    void tempToggle(boolean toggle)
    {
        if(speedometer_main_layout == null)return;

        if(toggle)
            activity.runOnUiThread(() -> speedometer_main_layout.setVisibility(View.VISIBLE));
        else {
            activity.runOnUiThread(() -> {
                speedometer_main_layout.setVisibility(View.GONE);
                deleteThreads();
            });
        }
    }

    Runnable runnable_turnlight_all = new Runnable() {
        @Override
        public void run() {
            while(!Thread.currentThread().isInterrupted())
            {
                activity.runOnUiThread(() -> {
                    if(turnlight_all_state) {
                        speedometr_blinker_icon.setImageTintList( null );
                        speedometr_turn_left.setImageResource(R.drawable.speedometr_turn_off);
                        speedometr_turn_right.setImageResource(R.drawable.speedometr_turn_off);
                        Samp.soundPool.play(turnlight_tick_sound_1, 0.2f, 0.1f, 1, 0, 1.0f);
                        turnlight_all_state = false;
                    }
                    else {
                        speedometr_blinker_icon.setImageTintList( ColorStateList.valueOf( Color.parseColor("#f44336") ) );
                        speedometr_turn_left.setImageResource(R.drawable.speedometr_turn_on);
                        speedometr_turn_right.setImageResource(R.drawable.speedometr_turn_on);
                        Samp.soundPool.play(turnlight_tick_sound_2, 0.2f, 0.1f, 1, 0, 1.0f);
                        turnlight_all_state = true;
                    }

                });
                try {
                    sleep(400);
                } catch (InterruptedException e) {
                    break;
                }
            }

        }
    };

    Runnable runnable_turnlight_left = new Runnable() {
        @Override
        public void run() {
            while(!Thread.currentThread().isInterrupted()) {
                activity.runOnUiThread(() -> {
                    if (turnlight_left_state) {
                        speedometr_turn_left.setImageResource(R.drawable.speedometr_turn_off);
                        Samp.soundPool.play(turnlight_tick_sound_1, 0.2f, 0.1f, 1, 0, 1.0f);
                        turnlight_left_state = false;
                    } else {
                        speedometr_turn_left.setImageResource(R.drawable.speedometr_turn_on);
                        Samp.soundPool.play(turnlight_tick_sound_2, 0.2f, 0.1f, 1, 0, 1.0f);
                        turnlight_left_state = true;
                    }

                });
                try {
                    sleep(400);
                } catch (InterruptedException e) {
                    break;
                }

            }

        }
    };

    Runnable runnable_turnlight_right = new Runnable() {
        @Override
        public void run() {
            while(!Thread.currentThread().isInterrupted())
            {
                activity.runOnUiThread(() -> {
                    if(turnlight_right_state) {
                        speedometr_turn_right.setImageResource(R.drawable.speedometr_turn_off);
                        Samp.soundPool.play(turnlight_tick_sound_1, 0.1f, 0.2f, 1, 0, 1.0f);
                        turnlight_right_state = false;
                    }
                    else {
                        speedometr_turn_right.setImageResource(R.drawable.speedometr_turn_on);
                        Samp.soundPool.play(turnlight_tick_sound_2, 0.1f, 0.2f, 1, 0, 1.0f);
                        turnlight_right_state = true;
                    }

                });
                try {
                    sleep(400);
                } catch (InterruptedException e) {
                    break;
                }
            }

        }
    };

}