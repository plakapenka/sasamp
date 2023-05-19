package com.ssmp.cr.gui;

import android.app.Activity;
import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.google.android.material.button.MaterialButton;
import com.ssmp.cr.R;

public class ShopStoreManager {

    private ConstraintLayout br_shopstroreselect_layout;
    private TextView shopstore_buyinfo;
    private ImageView shopstore_left;
    private ImageView shopstore_right;
    private MaterialButton shopstore_buy;
    private MaterialButton shopstore_exit;
    private ImageView shopstore_camera;

    native void onShopStoreClick(int buttonid);

    public ShopStoreManager(Activity activity){
        br_shopstroreselect_layout = activity.findViewById(R.id.br_shopstroreselect_layout);
        shopstore_buyinfo = activity.findViewById(R.id.shopstore_buyinfo);
        shopstore_left = activity.findViewById(R.id.shopstore_left);
        shopstore_right = activity.findViewById(R.id.shopstore_right);
        shopstore_buy = activity.findViewById(R.id.shopstore_buy);
        shopstore_exit = activity.findViewById(R.id.shopstore_exit);
        shopstore_camera = activity.findViewById(R.id.shopstore_camera);

        shopstore_left.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            onShopStoreClick(0);
        });

        shopstore_right.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            onShopStoreClick(3);
        });

        shopstore_buy.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            onShopStoreClick(1);
        });

        shopstore_exit.setOnClickListener(view -> {
            onShopStoreClick(2);
        });

        shopstore_camera.setOnClickListener(view -> {
            onShopStoreClick(4);
        });

        br_shopstroreselect_layout.setVisibility(View.GONE);
    }

    public void Toggle(boolean toggle, int type, int price)
    {
        if(!toggle)
        {
            br_shopstroreselect_layout.setVisibility(View.GONE);
            return;
        }
        String strprice = String.format("ЦЕНА: %s", price);
        shopstore_buyinfo.setText(strprice);
        if (type == 0) { shopstore_camera.setVisibility(View.VISIBLE); }
        else { shopstore_camera.setVisibility(View.GONE); }

        br_shopstroreselect_layout.setVisibility(View.VISIBLE);
    }

}