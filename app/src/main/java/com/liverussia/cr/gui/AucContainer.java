package com.liverussia.cr.gui;

import android.app.Activity;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.constraintlayout.widget.ConstraintSet;

import com.google.android.material.button.MaterialButton;
import com.liverussia.cr.R;
import com.nvidia.devtech.NvEventQueueActivity;

import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.Locale;

public class AucContainer {

    public native void Init();
    public native void SendClick(int buttonID);

    Activity activity;
    ConstraintLayout auc_main_layout;
    DecimalFormat price_format;
    TextView auc_price;
    TextView auc_caption;
    MaterialButton auc_give_butt;
    MaterialButton auc_sell_butt;
    View auc_exit_button;
    ImageView auc_item_img;

    public AucContainer(Activity activity){
        Init();
        this.activity = activity;
        DecimalFormatSymbols otherSymbols = new DecimalFormatSymbols(Locale.getDefault());
        otherSymbols.setGroupingSeparator('.');
        price_format = new DecimalFormat("###,###.###", otherSymbols);
        auc_main_layout = activity.findViewById(R.id.auc_main_layout);
        auc_main_layout.setVisibility(View.GONE);

        auc_price = activity.findViewById(R.id.auc_price);
        auc_caption = activity.findViewById(R.id.auc_caption);
        auc_give_butt = activity.findViewById(R.id.auc_give_butt);
        auc_give_butt.setOnClickListener(view -> {
            SendClick(1);
            auc_main_layout.setVisibility(View.GONE);
        });
        auc_sell_butt = activity.findViewById(R.id.auc_sell_butt);
        auc_sell_butt.setOnClickListener(view -> {
            SendClick(2);
            auc_main_layout.setVisibility(View.GONE);
        });
        auc_exit_button = activity.findViewById(R.id.auc_exit_button);
        auc_exit_button.setOnClickListener(view -> {
            SendClick(0);
            auc_main_layout.setVisibility(View.GONE);
        });
        auc_item_img = activity.findViewById(R.id.auc_item_img);
    }

    public void Show(int id, int type, int price){
        activity.runOnUiThread(() -> {
            auc_price.setText(price_format.format(price)+" руб.");
            int imgres = 0;

            if(type == 0){
                auc_caption.setText("Транспорт");
                imgres = activity.getResources().getIdentifier("auc_veh_"+id, "drawable", activity.getPackageName());
            }else {
                auc_caption.setText("Одежда");
                imgres = activity.getResources().getIdentifier("auc_skin_"+id, "drawable", activity.getPackageName());
            }
            auc_item_img.setImageResource(imgres);

            auc_main_layout.setVisibility(View.VISIBLE);
        });
    }

}
