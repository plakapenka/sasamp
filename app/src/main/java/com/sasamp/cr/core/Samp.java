package com.sasamp.cr.core;

import android.content.Context;
import android.media.AudioAttributes;
import android.media.SoundPool;
import android.os.Bundle;
import android.os.Vibrator;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.view.animation.AnimationUtils;
import android.widget.FrameLayout;

import androidx.constraintlayout.widget.ConstraintLayout;
import com.sasamp.cr.R;
import com.sasamp.cr.gui.HudManager;
import com.sasamp.cr.gui.Menu;
import com.sasamp.cr.gui.Notification;
import com.sasamp.cr.gui.dialogs.Dialog;
import com.sasamp.cr.gui.tab.Tab;
import com.sasamp.startMenu.GameMenuStart;

import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.Locale;
import java.util.Timer;
import java.util.TimerTask;

public class Samp extends GTASA
{
    public static native void playUrlSound(String url);
    public static WindowManager windowManager = null;
    public static DecimalFormat formatter = null;
    public static SoundPool soundPool = null;
    public static Vibrator vibrator = null;
    public static Samp activity = null;

    //fixme
    DialogClientSettings mDialogClientSettings = null;

    Menu mMenu = null;

    native void initSAMP(String game_path);
    public native void togglePlayer(int toggle);

    @Override
    public void onCreate(Bundle bundle)
    {
        super.onCreate(bundle);

        this.activity = this;
        windowManager = (WindowManager) this.getSystemService(WINDOW_SERVICE);

        new GameMenuStart(this);

        Log.i("java", "calling initSAMP");
        initSAMP(getExternalFilesDir(null).toString() + "/");

        init();
    }

    void init()
    {
        DecimalFormatSymbols otherSymbols = new DecimalFormatSymbols(Locale.getDefault());
        otherSymbols.setGroupingSeparator('.');
        formatter = new DecimalFormat("###,###.###", otherSymbols);

        AudioAttributes attributes = new AudioAttributes.Builder()
                .setUsage(AudioAttributes.USAGE_GAME)
                .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION)
                .build();
        soundPool = new SoundPool.Builder().setAudioAttributes(attributes).build();

        // FIXME
        new HudManager(this);
        new Tab(this);
        new Dialog(this);
        mMenu = new Menu(this);

        // mInputManager = new InputManager(this);
        //mHeightProvider = new HeightProvider(this).init(mRootFrame).setHeightListener(this);
        new Notification(this);
      //  new Colors(this);

        vibrator = (Vibrator) this.getSystemService(Context.VIBRATOR_SERVICE);

    }

    public void goVibrate(int milliseconds){
        if (vibrator.hasVibrator()) {
            vibrator.vibrate(milliseconds);
        }
    }

    public void showMenu() { runOnUiThread(() -> { mMenu.ShowMenu(); }); }

    public void ExitGame(){
        finishAndRemoveTask();
        System.exit(0);
    }

    public static Samp getInstance() {
        return activity;
    }

    public void showClientSettings()
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(mDialogClientSettings != null)
                {
                    mDialogClientSettings = null;
                }
                mDialogClientSettings = new DialogClientSettings();
                mDialogClientSettings.show(getSupportFragmentManager(), "test");
            }
        });
    }

    public void setPauseState(boolean z2) {
        FrameLayout mAndroidUI = findViewById(R.id.ui_layout);
        runOnUiThread(() -> mAndroidUI.setVisibility(z2 ? View.GONE:View.VISIBLE));
    }

    static void hideBackGroundSplash() {
        TimerTask task = new TimerTask() {
            public void run() {
                activity.runOnUiThread(() -> {
                    ConstraintLayout background_splash = activity.findViewById(R.id.background_splash);
                    background_splash.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.popup_hide_to_top));
                    background_splash.setVisibility(View.GONE);
                });
            }
        };
        Timer timer = new Timer("Timer");

        timer.schedule(task, 1000L);
    }
}
