package com.liverussia.cr.gui;

import android.app.Activity;
import android.view.animation.AnimationUtils;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.nvidia.devtech.NvEventQueueActivity;

public class ChooseSpawn {

    private FrameLayout br_choosespawn_layout;
    private ImageView spawn_fraction;
    private ImageView spawn_station;
    private ImageView spawn_exit;
    private ImageView spawn_garage;
    private ImageView spawn_house;
    private ImageView spawn_btn;

    int choosespawnid, organization, station, exit, garage, house;

    public ChooseSpawn(Activity activity){
        br_choosespawn_layout = activity.findViewById(R.id.br_choosespawn_layout);
        spawn_fraction = activity.findViewById(R.id.spawn_fraction);
        spawn_station = activity.findViewById(R.id.spawn_station);
        spawn_exit = activity.findViewById(R.id.spawn_exit);
        spawn_garage = activity.findViewById(R.id.spawn_garage);
        spawn_house = activity.findViewById(R.id.spawn_house);
        spawn_btn = activity.findViewById(R.id.spawn_btn);

        spawn_fraction.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            if (organization == 1)
            {
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
            if (station == 1)
            {
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
            if (exit == 1)
            {
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
            if (house == 1)
            {
                spawn_fraction.setImageResource(R.drawable.spawn_fraction);
                spawn_station.setImageResource(R.drawable.spawn_station);
                spawn_exit.setImageResource(R.drawable.spawn_exit);
                spawn_garage.setImageResource(R.drawable.spawn_garage);
                spawn_house.setImageResource(R.drawable.spawn_house_active);
                choosespawnid = 5;
            }
        });

        spawn_btn.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            if (choosespawnid > 0)
            {
                NvEventQueueActivity.getInstance().onChooseSpawnClick(choosespawnid);
            }
        });

        Utils.HideLayout(br_choosespawn_layout, false);
    }

    public void Show(int organizationstate, int stationstate, int exitstate, int garagestate, int housestate) {
        organization = organizationstate; station = stationstate; exit = exitstate; garage = garagestate; house = housestate;
        Utils.ShowLayout(br_choosespawn_layout, true);
    }

    public void Hide() {
        Utils.HideLayout(br_choosespawn_layout, true);
    }
}