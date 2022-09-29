package com.liverussia.cr.gui;

import android.app.Activity;
import android.view.animation.AnimationUtils;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.nvidia.devtech.NvEventQueueActivity;

public class FuelStationManager {

    private FrameLayout br_fuelstation_layout;
    private ConstraintLayout fuelstation_1;
    private ConstraintLayout fuelstation_2;
    private ConstraintLayout fuelstation_3;
    private ConstraintLayout fuelstation_4;
    private ConstraintLayout fuelstation_5;
    private ImageView fuelstation_exit;
    private ImageView fuelstation_buy;
    private TextView fuelstation_info;
    private TextView fuelstation_1_info;
    private TextView fuelstation_2_info;
    private TextView fuelstation_3_info;
    private TextView fuelstation_4_info;
    private TextView fuelstation_5_info;
    private TextView fuelstation_buyinfo;
    private TextView fuelstation_literinfo;
    private SeekBar fuelstation_bar;

    int fuelstation_active, fielstation_progress, fuelprice1, fuelprice2, fuelprice3, fuelprice4, fuelprice5, fuelprice;

    public FuelStationManager(Activity activity){
        br_fuelstation_layout = activity.findViewById(R.id.br_fuelstation_layout);
        fuelstation_exit = activity.findViewById(R.id.fuelstation_exit);
        fuelstation_info = activity.findViewById(R.id.fuelstation_info);
        fuelstation_1 = activity.findViewById(R.id.fuelstation_1);
        fuelstation_2 = activity.findViewById(R.id.fuelstation_2);
        fuelstation_3 = activity.findViewById(R.id.fuelstation_3);
        fuelstation_4 = activity.findViewById(R.id.fuelstation_4);
        fuelstation_5 = activity.findViewById(R.id.fuelstation_5);
        fuelstation_1_info = activity.findViewById(R.id.fuelstation_1_info);
        fuelstation_2_info = activity.findViewById(R.id.fuelstation_2_info);
        fuelstation_3_info = activity.findViewById(R.id.fuelstation_3_info);
        fuelstation_4_info = activity.findViewById(R.id.fuelstation_4_info);
        fuelstation_5_info = activity.findViewById(R.id.fuelstation_5_info);
        fuelstation_buyinfo = activity.findViewById(R.id.fuelstation_buyinfo);
        fuelstation_literinfo = activity.findViewById(R.id.fuelstation_literinfo);
        fuelstation_bar = activity.findViewById(R.id.fuelstation_bar);
        fuelstation_buy = activity.findViewById(R.id.fuelstation_buy);

        fuelstation_bar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                fielstation_progress = progress;
                if (fuelstation_active == 1) { fuelprice = fuelprice1 * progress; }
                else if (fuelstation_active == 2) { fuelprice = fuelprice2 * progress; }
                else if (fuelstation_active == 3) { fuelprice = fuelprice3 * progress; }
                else if (fuelstation_active == 4) { fuelprice = fuelprice4 * progress; }
                else if (fuelstation_active == 5) { fuelprice = fuelprice5 * progress; }
                String strpriceinfo = String.format("%s р", fuelprice);
                String strliterinfo = String.format("%s л", progress);
                fuelstation_buyinfo.setText(String.valueOf(strpriceinfo));
                fuelstation_literinfo.setText(String.valueOf(strliterinfo));
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        fuelstation_buy.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            Hide(fuelstation_active, fielstation_progress);
        });

        fuelstation_exit.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            Hide(0, 0);
        });

        fuelstation_1.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            fuelstation_1.setBackgroundResource(R.drawable.fuelstation_item_active_bg);
            fuelstation_2.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_3.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_4.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_5.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_active = 1;
            fuelprice = fuelprice1 * fuelstation_bar.getProgress();
            String strpriceinfo1 = String.format("%s р", fuelprice);
            fuelstation_buyinfo.setText(String.valueOf(strpriceinfo1));
        });

        fuelstation_2.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            fuelstation_1.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_2.setBackgroundResource(R.drawable.fuelstation_item_active_bg);
            fuelstation_3.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_4.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_5.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_active = 2;
            fuelprice = fuelprice2 * fuelstation_bar.getProgress();
            String strpriceinfo2 = String.format("%s р", fuelprice);
            fuelstation_buyinfo.setText(String.valueOf(strpriceinfo2));
        });

        fuelstation_3.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            fuelstation_1.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_2.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_3.setBackgroundResource(R.drawable.fuelstation_item_active_bg);
            fuelstation_4.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_5.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_active = 3;
            fuelprice = fuelprice3 * fuelstation_bar.getProgress();
            String strpriceinfo3 = String.format("%s р", fuelprice);
            fuelstation_buyinfo.setText(String.valueOf(strpriceinfo3));
        });

        fuelstation_4.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            fuelstation_1.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_2.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_3.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_4.setBackgroundResource(R.drawable.fuelstation_item_active_bg);
            fuelstation_5.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_active = 4;
            fuelprice = fuelprice4 * fuelstation_bar.getProgress();
            String strpriceinfo4 = String.format("%s р", fuelprice);
            fuelstation_buyinfo.setText(String.valueOf(strpriceinfo4));
        });

        fuelstation_5.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            fuelstation_1.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_2.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_3.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_4.setBackgroundResource(R.drawable.fuelstation_item_bg);
            fuelstation_5.setBackgroundResource(R.drawable.fuelstation_item_active_bg);
            fuelstation_active = 5;
            fuelprice = fuelprice5 * fuelstation_bar.getProgress();
            String strpriceinfo5 = String.format("%s р", fuelprice);
            fuelstation_buyinfo.setText(String.valueOf(strpriceinfo5));
        });

        Utils.HideLayout(br_fuelstation_layout, false);
    }

    public void Show(int type, int price1, int price2, int price3, int price4, int price5) {
        fuelprice1 = price1;
        fuelprice2 = price2;
        fuelprice3 = price3;
        fuelprice4 = price5;
        fuelprice5 = price4;

        String strprice1 = String.format("%sр/литр", price1);
        String strprice2 = String.format("%sр/литр", price2);
        String strprice3 = String.format("%sр/литр", price3);
        String strprice4 = String.format("%sр/литр", price5);
        String strprice5 = String.format("%sр/литр", price4);
        fuelstation_1_info.setText(String.valueOf(strprice1));
        fuelstation_2_info.setText(String.valueOf(strprice2));
        fuelstation_3_info.setText(String.valueOf(strprice3));
        fuelstation_4_info.setText(String.valueOf(strprice5));
        fuelstation_5_info.setText(String.valueOf(strprice4));
        fuelstation_1.setBackgroundResource(R.drawable.fuelstation_item_bg);
        fuelstation_2.setBackgroundResource(R.drawable.fuelstation_item_bg);
        fuelstation_3.setBackgroundResource(R.drawable.fuelstation_item_bg);
        fuelstation_4.setBackgroundResource(R.drawable.fuelstation_item_bg);
        fuelstation_5.setBackgroundResource(R.drawable.fuelstation_item_bg);

        fuelstation_buyinfo.setText("0 р");
        fuelstation_literinfo.setText("0 л");
        fuelstation_bar.setProgress(0);

        fuelstation_active = type;
        if (type == 1) {
            fuelstation_info.setText("Рекомендуемый тип топлива: АИ-92");
            fuelstation_1.setBackgroundResource(R.drawable.fuelstation_item_active_bg);
        }
        else if (type == 2) {
            fuelstation_info.setText("Рекомендуемый тип топлива: АИ-95");
            fuelstation_2.setBackgroundResource(R.drawable.fuelstation_item_active_bg);
        }
        else if (type == 3) {
            fuelstation_info.setText("Рекомендуемый тип топлива: АИ-98");
            fuelstation_3.setBackgroundResource(R.drawable.fuelstation_item_active_bg);
        }
        else if (type == 4) {
            fuelstation_info.setText("Рекомендуемый тип топлива: ДТ");
            fuelstation_4.setBackgroundResource(R.drawable.fuelstation_item_active_bg);
        }
        else if (type == 5) {
            fuelstation_info.setText("Рекомендуемый тип топлива: АИ-100");
            fuelstation_5.setBackgroundResource(R.drawable.fuelstation_item_active_bg);
        }

        Utils.ShowLayout(br_fuelstation_layout, true);
    }

    public void Hide(int typefuel, int literfuel) {
        NvEventQueueActivity.getInstance().onFuelStationClick(typefuel, literfuel);
        Utils.HideLayout(br_fuelstation_layout, true);
    }
}