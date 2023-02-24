package com.liverussia.cr.core;

import android.content.Context;
import android.media.AudioAttributes;
import android.media.AudioManager;
import android.media.SoundPool;
import android.os.Bundle;
import android.util.Log;

import com.liverussia.cr.gui.AdminRecon;
import com.liverussia.cr.gui.ArmyGameManager;
import com.liverussia.cr.gui.AucContainer;
import com.liverussia.cr.gui.AuthorizationManager;
import com.liverussia.cr.gui.AutoShop;
import com.liverussia.cr.gui.Casino;
import com.liverussia.cr.gui.CasinoBaccarat;
import com.liverussia.cr.gui.CasinoDice;
import com.liverussia.cr.gui.Casino_LuckyWheel;
import com.liverussia.cr.gui.ChooseServer;
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
import com.liverussia.cr.gui.donate.Donate;
import com.liverussia.cr.gui.tab.Tab;

import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.Locale;

public class Samp extends GTASA
{
    public static native void playUrlSound(String url);
    public static DecimalFormat formatter = null;
    public static SoundPool soundPool = null;

    native void initSAMP(String game_path);

    @Override
    public void onCreate(Bundle bundle)
    {

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

        new HudManager(this);
        new CasinoDice(this);
        new CasinoBaccarat(this);
        new AucContainer(this);
        new DailyReward(this);
        new Tab(this);
        new Casino(this);

        //
//        new ChooseServer(this);
//        new Furniture_factory(this);
//        new AdminRecon(this);
//        new DuelsHud(this);
//        new TechIspect(this);
//        // mInputManager = new InputManager(this);
//        //mHeightProvider = new HeightProvider(this).init(mRootFrame).setHeightListener(this);
//        new Notification(this);
//        new AuthorizationManager(this);
//        new RegistrationManager(this);
//        new FuelStationManager(this);
//        new OilFactoryManager(this);
//       // vibrator = (Vibrator) this.getSystemService(Context.VIBRATOR_SERVICE);
//        new ArmyGameManager(this);
//        new ShopStoreManager(this);
//        new GunShopManager(this);
//        new PreDeath(this);
//        new Dialog(this);
//        new Inventory(this);
//        new MineGame1(this);
//        new MineGame2(this);
//        new MineGame3(this);
//        new Casino_LuckyWheel(this);
//
//        new SamwillManager(this);
//        new AutoShop(this);
//
//        new Menu(this);
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
}
