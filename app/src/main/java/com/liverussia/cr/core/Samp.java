package com.liverussia.cr.core;

import android.content.Context;
import android.media.AudioAttributes;
import android.media.AudioManager;
import android.media.SoundPool;
import android.os.Bundle;
import android.os.Vibrator;
import android.util.Log;
import android.view.View;
import android.widget.FrameLayout;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.AdminRecon;
import com.liverussia.cr.gui.ArmyGameManager;
import com.liverussia.cr.gui.AucContainer;
import com.liverussia.cr.gui.AutoShop;
import com.liverussia.cr.gui.Casino;
import com.liverussia.cr.gui.CasinoBaccarat;
import com.liverussia.cr.gui.CasinoDice;
import com.liverussia.cr.gui.Casino_LuckyWheel;
import com.liverussia.cr.gui.ChooseSpawn;
import com.liverussia.cr.gui.DailyReward;
import com.liverussia.cr.gui.DuelsHud;
import com.liverussia.cr.gui.FuelStationManager;
import com.liverussia.cr.gui.Furniture_factory;
import com.liverussia.cr.gui.GunShopManager;
import com.liverussia.cr.gui.HudManager;
import com.liverussia.cr.gui.Inventory;
import com.liverussia.cr.gui.Menu;
import com.liverussia.cr.gui.MineGame1;
import com.liverussia.cr.gui.MineGame2;
import com.liverussia.cr.gui.MineGame3;
import com.liverussia.cr.gui.Notification;
import com.liverussia.cr.gui.OilFactoryManager;
import com.liverussia.cr.gui.PreDeath;
import com.liverussia.cr.gui.RegistrationManager;
import com.liverussia.cr.gui.SamwillManager;
import com.liverussia.cr.gui.ShopStoreManager;
import com.liverussia.cr.gui.TechIspect;
import com.liverussia.cr.gui.dialogs.Dialog;
import com.liverussia.cr.gui.tab.Tab;
import com.nvidia.devtech.NvEventQueueActivity;

import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.Locale;

public class Samp extends GTASA
{
    public static native void playUrlSound(String url);
    public static DecimalFormat formatter = null;
    public static SoundPool soundPool = null;
    public static Vibrator vibrator = null;
    private static Samp thiz = null;

    //fixme
    AutoShop mAutoShop = null;
    RegistrationManager mRegistrationManager = null;
    FuelStationManager mFuelStationManager = null;
    OilFactoryManager mOilFactoryManager = null;
    SamwillManager mSamwillManager = null;
    DialogClientSettings mDialogClientSettings = null;

    ArmyGameManager mArmyGameManager = null;
    ShopStoreManager mShopStoreManager = null;
    GunShopManager mGunShopManager = null;
    ChooseSpawn mChooseSpawn = null;
    Menu mMenu = null;

    native void initSAMP(String game_path);
    public native void togglePlayer(int toggle);

    @Override
    public void onCreate(Bundle bundle)
    {
        thiz = this;

        Log.i("java", "calling initSAMP");
        initSAMP(getExternalFilesDir(null).toString() + "/");

        super.onCreate(bundle);

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
        mRegistrationManager = new RegistrationManager(this);
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

        vibrator = (Vibrator) this.getSystemService(Context.VIBRATOR_SERVICE);
    }

    public void playLocalSound(int soundID, float speed){
        soundPool.load(this, soundID, 0);

        soundPool.setOnLoadCompleteListener(new SoundPool.OnLoadCompleteListener() {
            @Override
            public void onLoadComplete(SoundPool soundPool, int i, int i1) {
                AudioManager audioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);

                float curVolume = audioManager.getStreamVolume(AudioManager.STREAM_MUSIC);
                float maxVolume = audioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
                float leftVolume = curVolume / maxVolume;
                float rightVolume = curVolume / maxVolume;

                soundPool.play(i, leftVolume, rightVolume, 1, 0, speed);

                soundPool.unload(i);
            }
        });
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
    public void showRegistration(String nick, int id) { runOnUiThread(() -> { mRegistrationManager.Show(nick, id); }); }

    public void hideRegistration() { runOnUiThread(() -> { mRegistrationManager.Hide(); }); }

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
        return thiz;
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
}
