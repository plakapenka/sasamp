package com.liverussia.cr.gui;

import android.app.Activity;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;
import com.nvidia.devtech.NvEventQueueActivity;

import java.util.Date;

public class Casino_LuckyWheel {
    Activity activity;
    ConstraintLayout casino_luckywheel_mainlayout;
    TextView casino_luckywheel_count;
    Button casino_luckywheel_speen_pay_butt;
    Button casino_luckywheel_speen_free_butt;
    View casino_luckywheel_exit_butt;
    ConstraintLayout hud_main;

    public native void ClickButt(int buttonID);
    public native void Casino_LuckyWheel();

    public Casino_LuckyWheel(Activity aactivity)
    {
        activity = aactivity;
        hud_main = activity.findViewById(R.id.hud_main);
        casino_luckywheel_mainlayout = activity.findViewById(R.id.casino_luckywheel_mainlayout);
        casino_luckywheel_mainlayout.setVisibility(View.GONE);

        casino_luckywheel_count = activity.findViewById(R.id.casino_luckywheel_count);
        casino_luckywheel_speen_pay_butt = activity.findViewById(R.id.casino_luckywheel_speen_pay_butt);
        casino_luckywheel_speen_free_butt = activity.findViewById(R.id.casino_luckywheel_speen_free_butt);
        casino_luckywheel_exit_butt = activity.findViewById(R.id.casino_luckywheel_exit_butt);


        casino_luckywheel_speen_free_butt.setOnClickListener(view -> {
            ClickButt(0);
            Hide();
        });
        casino_luckywheel_speen_pay_butt.setOnClickListener(view -> {
            ClickButt(1);
            Hide();
        });
        casino_luckywheel_exit_butt.setOnClickListener(view -> {
            Hide();
        });

    }
    public void Show(int count, int time) {
        String timeleft = new java.text.SimpleDateFormat("HH:mm").format(new Date(time * 1000));
        activity.runOnUiThread(() ->
        {
            hud_main.setVisibility(View.GONE);
            casino_luckywheel_count.setText(String.format("%d", count));
            if(count <= 0) {
                casino_luckywheel_speen_free_butt.setText("Доступно через " + timeleft);
            }
            else {
                casino_luckywheel_speen_free_butt.setText("бесплатная прокрутка");
            }
            casino_luckywheel_mainlayout.setVisibility(View.VISIBLE);
        });
    }
    public void Hide()
    {
        activity.runOnUiThread(() -> {
            hud_main.setVisibility(View.VISIBLE);
            casino_luckywheel_mainlayout.setVisibility(View.GONE);
        });
    }
}
