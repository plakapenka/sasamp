package com.liverussia.cr.gui;

import android.app.Activity;
import android.view.View;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;

public class CasinoBaccarat {
    static final int CHIP_TYPE_1 = 1;
    static final int CHIP_TYPE_5 = 2;
    static final int CHIP_TYPE_25 = 3;
    static final int CHIP_TYPE_100 = 4;
    static final int CHIP_TYPE_500 = 5;
    static final int CHIP_TYPE_1000 = 6;


    private int selected_chip = 0;
    private int current_bet = 0;

    Activity activity;
    ConstraintLayout casino_bc_main_layout;
    ConstraintLayout casino_bc_red_area;
    ConstraintLayout casino_bc_green_area;
    ConstraintLayout casino_bc_yellow_area;
    ConstraintLayout casino_bc_red_chip;
    ConstraintLayout casino_bc_yellow_chip;

    private native void addBet(int sum);

    public CasinoBaccarat(Activity activity){
        this.activity = activity;

        casino_bc_main_layout = activity.findViewById(R.id.casino_bc_main_layout);
        casino_bc_main_layout.setVisibility(View.GONE);

        casino_bc_red_area = activity.findViewById(R.id.casino_bc_red_area);
        casino_bc_red_area.setOnClickListener(view -> {
            if(selected_chip == 0)return;
            addBet( getChipValue(selected_chip) );

        });

        casino_bc_green_area = activity.findViewById(R.id.casino_bc_green_area);
        casino_bc_green_area.setOnClickListener(view -> {

        });

        casino_bc_yellow_area = activity.findViewById(R.id.casino_bc_yellow_area);
        casino_bc_yellow_area.setOnClickListener(view -> {

        });

        casino_bc_red_chip = activity.findViewById(R.id.casino_bc_red_chip);
        casino_bc_yellow_chip = activity.findViewById(R.id.casino_bc_yellow_chip);
    }

    void show(){
        activity.runOnUiThread(() -> {
            casino_bc_main_layout.setVisibility(View.VISIBLE);
        });
    }

    int getChipValue(int chipType){
        switch (chipType) {
            case CHIP_TYPE_1:       return 1;
            case CHIP_TYPE_5:       return 5;
            case CHIP_TYPE_25:      return 25;
            case CHIP_TYPE_100:     return 100;
            case CHIP_TYPE_500:     return 500;
            case CHIP_TYPE_1000:    return 1000;
            default:                return 0;
        }
    }

    void reset(){
        activity.runOnUiThread(() -> {
            casino_bc_yellow_chip.setVisibility(View.GONE);
            casino_bc_red_chip.setVisibility(View.GONE);

            selected_chip = 0;
        });
    }
}
