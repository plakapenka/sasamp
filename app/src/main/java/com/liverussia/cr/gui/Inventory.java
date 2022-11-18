package com.liverussia.cr.gui;

import android.app.Activity;
import android.view.View;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;


public class Inventory {
    private Activity activity;
    private ConstraintLayout inv_main_layout;

    public Inventory(Activity activity)
    {
        this.activity = activity;

        InventoryInit();

        inv_main_layout = activity.findViewById(R.id.inv_main_layout);
        inv_main_layout.setVisibility(View.GONE);
    }

    public void ToggleShow(boolean toggle)
    {
        if(toggle){
            activity.runOnUiThread(() -> {
                inv_main_layout.setVisibility(View.VISIBLE);
            });
        }else{
            activity.runOnUiThread(() -> {
                inv_main_layout.setVisibility(View.GONE);
            });
        }

    }
    public native void InventoryInit();
}
