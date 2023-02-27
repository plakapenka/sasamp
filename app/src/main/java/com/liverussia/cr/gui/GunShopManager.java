package com.liverussia.cr.gui;

import android.app.Activity;
import android.view.animation.AnimationUtils;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.nvidia.devtech.NvEventQueueActivity;

public class GunShopManager {

    private FrameLayout br_gunshop_layout;
    private ConstraintLayout gunshop_golfclub;
    private ConstraintLayout gunshop_mp5;
    private ConstraintLayout gunshop_m4;
    private ConstraintLayout gunshop_deagle;
    private ConstraintLayout gunshop_rifle;
    private ConstraintLayout gunshop_smoke;
    private ConstraintLayout gunshop_ak47;
    private ConstraintLayout gunshop_bat;
    private ConstraintLayout gunshop_shotgun;
    private ConstraintLayout gunshop_armor;
    private ConstraintLayout gunshop_katana;
    private ConstraintLayout gunshop_knucles;
    private TextView gunshop_weapontext;
    private ImageView gunshop_weaponimage;
    private ImageView gunshop_weaponbuy;
    private ImageView gunshop_exit;

    int weaponselectid;

    public GunShopManager(Activity activity){
        br_gunshop_layout = activity.findViewById(R.id.br_gunshop_layout);
        gunshop_weapontext = activity.findViewById(R.id.gunshop_weapontext);
        gunshop_weaponimage = activity.findViewById(R.id.gunshop_weaponimage);
        gunshop_weaponbuy = activity.findViewById(R.id.gunshop_weaponbuy);
        gunshop_golfclub = activity.findViewById(R.id.gunshop_golfclub);
        gunshop_mp5 = activity.findViewById(R.id.gunshop_mp5);
        gunshop_m4 = activity.findViewById(R.id.gunshop_m4);
        gunshop_deagle = activity.findViewById(R.id.gunshop_deagle);
        gunshop_rifle = activity.findViewById(R.id.gunshop_rifle);
        gunshop_smoke = activity.findViewById(R.id.gunshop_smoke);
        gunshop_ak47 = activity.findViewById(R.id.gunshop_ak47);
        gunshop_bat = activity.findViewById(R.id.gunshop_bat);
        gunshop_shotgun = activity.findViewById(R.id.gunshop_shotgun);
        gunshop_armor = activity.findViewById(R.id.gunshop_armor);
        gunshop_katana = activity.findViewById(R.id.gunshop_katana);
        gunshop_knucles = activity.findViewById(R.id.gunshop_knucles);
        gunshop_exit = activity.findViewById(R.id.gunshop_exit);

        gunshop_exit.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            Hide();
        });

        gunshop_weaponbuy.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            NvEventQueueActivity.getInstance().onGunShopClick(weaponselectid);
        });

        gunshop_golfclub.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            weaponselectid = 1;
            gunshop_weapontext.setText("ГОЛЬФ КЛЮШКА");
            gunshop_weaponimage.setImageResource(R.drawable.weapon_2);
        });

        gunshop_mp5.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            weaponselectid = 2;
            gunshop_weapontext.setText("MP5");
            gunshop_weaponimage.setImageResource(R.drawable.weapon_29);
        });

        gunshop_m4.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            weaponselectid = 3;
            gunshop_weapontext.setText("M4");
            gunshop_weaponimage.setImageResource(R.drawable.weapon_31);
        });

        gunshop_deagle.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            weaponselectid = 4;
            gunshop_weapontext.setText("DESERT EAGLE");
            gunshop_weaponimage.setImageResource(R.drawable.weapon_24);
        });

        gunshop_rifle.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            weaponselectid = 5;
            gunshop_weapontext.setText("RIFLE");
            gunshop_weaponimage.setImageResource(R.drawable.weapon_33);
        });

        gunshop_smoke.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            weaponselectid = 6;
            gunshop_weapontext.setText("ДЫМОВАЯ ШАШКА");
            gunshop_weaponimage.setImageResource(R.drawable.weapon_17);
        });

        gunshop_ak47.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            weaponselectid = 7;
            gunshop_weapontext.setText("AK-47");
            gunshop_weaponimage.setImageResource(R.drawable.weapon_30);
        });

        gunshop_bat.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            weaponselectid = 8;
            gunshop_weapontext.setText("БИТА");
            gunshop_weaponimage.setImageResource(R.drawable.weapon_5);
        });

        gunshop_shotgun.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            weaponselectid = 9;
            gunshop_weapontext.setText("SHOTGUN");
            gunshop_weaponimage.setImageResource(R.drawable.weapon_25);
        });

        gunshop_armor.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            weaponselectid = 10;
            gunshop_weapontext.setText("БРОНЕЖИЛЕТ");
            gunshop_weaponimage.setImageResource(R.drawable.gunshop_armor);
        });

        gunshop_katana.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            weaponselectid = 11;
            gunshop_weapontext.setText("КАТАНА");
            gunshop_weaponimage.setImageResource(R.drawable.weapon_8);
        });

        gunshop_knucles.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            weaponselectid = 12;
            gunshop_weapontext.setText("КАСТЕТ");
            gunshop_weaponimage.setImageResource(R.drawable.weapon_1);
        });

        Utils.HideLayout(br_gunshop_layout, false);
    }

    public void Show() {
        weaponselectid = 12;
        gunshop_weapontext.setText("КАСТЕТ");
        gunshop_weaponimage.setImageResource(R.drawable.weapon_1);
        Utils.ShowLayout(br_gunshop_layout, true);
    }

    public void Hide() {
        NvEventQueueActivity.getInstance().onGunShopClick(0);
        Utils.HideLayout(br_gunshop_layout, true);
    }
}