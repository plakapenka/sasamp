package com.ssmp.cr.core;

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
import com.ssmp.cr.R;
import com.ssmp.cr.gui.AdminRecon;
import com.ssmp.cr.gui.ArmyGameManager;
import com.ssmp.cr.gui.AucContainer;
import com.ssmp.cr.gui.AutoShop;
import com.ssmp.cr.gui.Casino;
import com.ssmp.cr.gui.CasinoBaccarat;
import com.ssmp.cr.gui.CasinoDice;
import com.ssmp.cr.gui.Casino_LuckyWheel;
import com.ssmp.cr.gui.DailyReward;
import com.ssmp.cr.gui.DuelsHud;
import com.ssmp.cr.gui.FuelStationManager;
import com.ssmp.cr.gui.Furniture_factory;
import com.ssmp.cr.gui.GunShopManager;
import com.ssmp.cr.gui.HudManager;
import com.ssmp.cr.gui.Inventory;
import com.ssmp.cr.gui.Menu;
import com.ssmp.cr.gui.MineGame1;
import com.ssmp.cr.gui.MineGame2;
import com.ssmp.cr.gui.MineGame3;
import com.ssmp.cr.gui.Notification;
import com.ssmp.cr.gui.OilFactoryManager;
import com.ssmp.cr.gui.PreDeath;
import com.ssmp.cr.gui.SamwillManager;
import com.ssmp.cr.gui.ShopStoreManager;
import com.ssmp.cr.gui.TechIspect;
import com.ssmp.cr.gui.dialogs.Dialog;
import com.ssmp.cr.gui.tab.Tab;
import com.ssmp.startMenu.GameMenuStart;

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
    AutoShop mAutoShop = null;
    FuelStationManager mFuelStationManager = null;
    OilFactoryManager mOilFactoryManager = null;
    SamwillManager mSamwillManager = null;
    DialogClientSettings mDialogClientSettings = null;

    ArmyGameManager mArmyGameManager = null;
    ShopStoreManager mShopStoreManager = null;
    GunShopManager mGunShopManager = null;
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
        new CasinoDice(this);
        new CasinoBaccarat(this);
        new AucContainer(this);
        new DailyReward(this);
        new Tab(this);
        new Casino(this);
        new PreDeath(this);
        new Dialog(this);
        new Inventory(this);
        new MineGame1(this);
        new MineGame2(this);
        new MineGame3(this);
        new Casino_LuckyWheel(this);
        mAutoShop = new AutoShop(this);
        mFuelStationManager = new FuelStationManager(this);
        mOilFactoryManager = new OilFactoryManager(this);

        mArmyGameManager = new ArmyGameManager(this);
        mShopStoreManager = new ShopStoreManager(this);
        mGunShopManager = new GunShopManager(this);

        mSamwillManager = new SamwillManager(this);
        mMenu = new Menu(this);

        new Furniture_factory(this);
        new AdminRecon(this);
        new DuelsHud(this);
        new TechIspect(this);
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


    // fixme
    public void toggleAutoShop(boolean toggle) {
        runOnUiThread(() -> mAutoShop.ToggleShow(toggle));
    }
    public void updateAutoShop(String name, int price, int count, float maxspeed, float acceleration, int gear) {
        runOnUiThread(() -> mAutoShop.Update(name, price, count, maxspeed, acceleration, gear));
    }

    public void showMenu() { runOnUiThread(() -> { mMenu.ShowMenu(); }); }


    public void showSamwill() { runOnUiThread(() -> { mSamwillManager.Show(); }); }

    public void ExitGame(){
        finishAndRemoveTask();
        System.exit(0);
    }

    public void showOilFactoryGame() { runOnUiThread(() -> { mOilFactoryManager.Show(); } ); }

    public void showArmyGame(int quantity) { runOnUiThread(() -> { mArmyGameManager.Show(quantity); } ); }

    public void hideArmyGame() { runOnUiThread(() -> { mArmyGameManager.HideFull(); } ); }

    public void toggleShopStoreManager(boolean toggle, int type, int price) { runOnUiThread(() -> mShopStoreManager.Toggle(toggle, type, price) ); }

    public void showGunShopManager() { runOnUiThread(() -> { mGunShopManager.Show(); } ); }

    public void hideGunShopManager() { runOnUiThread(() -> { mGunShopManager.Hide(); } ); }

    public void showFuelStation(int type, int price1, int price2, int price3, int price4, int price5, int maxCount) { runOnUiThread(() -> { mFuelStationManager.Show(type, price1, price2, price3, price4, price5, maxCount); } ); }

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
