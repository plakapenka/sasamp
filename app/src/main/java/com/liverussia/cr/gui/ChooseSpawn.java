package com.liverussia.cr.gui;

import android.app.Activity;
import android.view.animation.AnimationUtils;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.google.android.material.button.MaterialButton;
import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.nvidia.devtech.NvEventQueueActivity;

public class ChooseSpawn {

    FrameLayout br_choosespawn_layout;
    ImageView spawn_fraction;
    ImageView spawn_station;
    ImageView spawn_exit;
    ImageView spawn_garage;
    ImageView spawn_house;
    MaterialButton spawn_btn;
    Activity activity;

    int choosespawnid, organization, station, exit, garage, house;

    native void clickEnter(int spawnId);

    public ChooseSpawn(Activity activity)
    {

        this.activity = activity;
        br_choosespawn_layout = activity.findViewById(R.id.br_choosespawn_layout);
        spawn_fraction = activity.findViewById(R.id.spawn_fraction);
        spawn_station = activity.findViewById(R.id.spawn_station);
        spawn_exit = activity.findViewById(R.id.spawn_exit);
        spawn_garage = activity.findViewById(R.id.spawn_garage);
        spawn_house = activity.findViewById(R.id.spawn_house);
        spawn_btn = activity.findViewById(R.id.spawn_btn);

        activity.runOnUiThread(()->
        {
            spawn_fraction.setOnClickListener(view -> {
                view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
                if (organization == 1) {
                    spawn_fraction.setImageResource(R.drawable.spawn_fraction_active);
                    spawn_station.setImageResource(R.drawable.spawn_station);
                    spawn_exit.setImageResource(R.drawable.spawn_exit);
                    spawn_garage.setImageResource(R.drawable.spawn_garage);
                    spawn_house.setImageResource(R.drawable.spawn_house);
                    choosespawnid = 1;
                }
            });

            spawn_station.setOnClickListener(view -> {
                view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
                if (station == 1) {
                    spawn_fraction.setImageResource(R.drawable.spawn_fraction);
                    spawn_station.setImageResource(R.drawable.spawn_station_active);
                    spawn_exit.setImageResource(R.drawable.spawn_exit);
                    spawn_garage.setImageResource(R.drawable.spawn_garage);
                    spawn_house.setImageResource(R.drawable.spawn_house);
                    choosespawnid = 2;
                }
            });

            spawn_exit.setOnClickListener(view -> {
                view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
                if (exit == 1) {
                    spawn_fraction.setImageResource(R.drawable.spawn_fraction);
                    spawn_station.setImageResource(R.drawable.spawn_station);
                    spawn_exit.setImageResource(R.drawable.spawn_exit_active);
                    spawn_garage.setImageResource(R.drawable.spawn_garage);
                    spawn_house.setImageResource(R.drawable.spawn_house);
                    choosespawnid = 3;
                }
            });

            spawn_garage.setOnClickListener(view -> {
                view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
                if (garage == 1) {
                    spawn_fraction.setImageResource(R.drawable.spawn_fraction);
                    spawn_station.setImageResource(R.drawable.spawn_station);
                    spawn_exit.setImageResource(R.drawable.spawn_exit);
                    spawn_garage.setImageResource(R.drawable.spawn_garage_active);
                    spawn_house.setImageResource(R.drawable.spawn_house);
                    choosespawnid = 4;
                }
            });

            spawn_house.setOnClickListener(view -> {
                view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
                if (house == 1) {
                    spawn_fraction.setImageResource(R.drawable.spawn_fraction);
                    spawn_station.setImageResource(R.drawable.spawn_station);
                    spawn_exit.setImageResource(R.drawable.spawn_exit);
                    spawn_garage.setImageResource(R.drawable.spawn_garage);
                    spawn_house.setImageResource(R.drawable.spawn_house_active);
                    choosespawnid = 5;
                }
            });

            spawn_btn.setOnClickListener(view -> {
                if (choosespawnid > 0) {
                    clickEnter(choosespawnid);
                }
            });

        });

        Utils.HideLayout(br_choosespawn_layout, false);
    }

    void show(int organizationstate, int stationstate, int exitstate, int garagestate, int housestate)
    {
        activity.runOnUiThread(()-> {
            organization = organizationstate;
            station = stationstate;
            exit = exitstate;
            garage = garagestate;
            house = housestate;

            Utils.ShowLayout(br_choosespawn_layout, true);
        });
    }

    public void hide()
    {
        activity.runOnUiThread(()-> {
            Utils.HideLayout(br_choosespawn_layout, true);
        });
    }
}