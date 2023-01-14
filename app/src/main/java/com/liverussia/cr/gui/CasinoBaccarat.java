package com.liverussia.cr.gui;

import android.app.Activity;
import android.view.View;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;

public class CasinoBaccarat {
    private static final int CHIP_TYPE_1 = 1;
    private static final int CHIP_TYPE_5 = 2;
    private static final int CHIP_TYPE_25 = 3;
    private static final int CHIP_TYPE_100 = 4;
    private static final int CHIP_TYPE_500 = 5;
    private static final int CHIP_TYPE_1000 = 6;

    Activity activity;
    ConstraintLayout casino_bc_main_layout;
    ConstraintLayout casino_bc_red_area;
    ConstraintLayout casino_bc_green_area;
    ConstraintLayout casino_bc_yellow_area;

    public CasinoBaccarat(Activity activity){
        this.activity = activity;

        casino_bc_main_layout = activity.findViewById(R.id.casino_bc_main_layout);
        casino_bc_main_layout.setVisibility(View.GONE);

        casino_bc_red_area = activity.findViewById(R.id.casino_bc_red_area);
        casino_bc_red_area.setOnClickListener(view -> {

        });

        casino_bc_green_area = activity.findViewById(R.id.casino_bc_green_area);
        casino_bc_green_area.setOnClickListener(view -> {

        });

        casino_bc_yellow_area = activity.findViewById(R.id.casino_bc_yellow_area);
        casino_bc_yellow_area.setOnClickListener(view -> {

        });
    }
}
