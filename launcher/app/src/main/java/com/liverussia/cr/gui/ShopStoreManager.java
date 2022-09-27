package com.liverussia.cr.gui;

import android.app.Activity;
import android.view.animation.AnimationUtils;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.nvidia.devtech.NvEventQueueActivity;

public class ShopStoreManager {

    private FrameLayout br_shopstroreselect_layout;
    private TextView shopstore_buyinfo;
    private ImageView shopstore_left;
    private ImageView shopstore_right;
    private ImageView shopstore_buy;
    private ImageView shopstore_exit;
    private ImageView shopstore_camera;

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
            NvEventQueueActivity.getInstance().onShopStoreClick(0);
        });

        shopstore_right.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            NvEventQueueActivity.getInstance().onShopStoreClick(3);
        });

        shopstore_buy.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            NvEventQueueActivity.getInstance().onShopStoreClick(1);
        });

        shopstore_exit.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            NvEventQueueActivity.getInstance().onShopStoreClick(2);
        });

        shopstore_camera.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            NvEventQueueActivity.getInstance().onShopStoreClick(4);
        });

        Utils.HideLayout(br_shopstroreselect_layout, false);
    }

    public void Show(int type, int price) {
        String strprice = String.format("ЦЕНА: %s", price);
        shopstore_buyinfo.setText(String.valueOf(strprice));
        if (type == 0) { Utils.ShowLayout(shopstore_camera, false); }
        else { Utils.HideLayout(shopstore_camera, false); }
        Utils.ShowLayout(br_shopstroreselect_layout, true);
    }

    public void Hide() {
        Utils.HideLayout(br_shopstroreselect_layout, true);
    }
}