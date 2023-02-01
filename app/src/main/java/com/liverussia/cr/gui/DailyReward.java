package com.liverussia.cr.gui;

import android.app.Activity;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.text.format.DateUtils;
import android.view.View;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.liverussia.launcher.ui.activity.MainActivity;

import java.util.ArrayList;
import java.util.List;

public class DailyReward {

    Activity activity;
    ConstraintLayout daily_main_layout;
    View daily_exit_butt;

    List<ConstraintLayout>daily_items = new ArrayList<>();

    native void init();
    native void click();
    native void hide();

    public DailyReward(Activity activity)
    {
        init();

        this.activity = activity;
        daily_main_layout = activity.findViewById(R.id.daily_main_layout);

        daily_exit_butt = activity.findViewById(R.id.daily_exit_butt);
        daily_exit_butt.setOnClickListener(view -> {
            activity.runOnUiThread(() -> {
                daily_main_layout.setVisibility(View.GONE);
                hide();
            });
        });

        daily_items.add( activity.findViewById(R.id.daily_item_bg_1) );
        daily_items.add( activity.findViewById(R.id.daily_item_bg_2) );
        daily_items.add( activity.findViewById(R.id.daily_item_bg_3) );
        daily_items.add( activity.findViewById(R.id.daily_item_bg_4) );
        daily_items.add( activity.findViewById(R.id.daily_item_bg_5) );
        daily_items.add( activity.findViewById(R.id.daily_item_bg_6) );
        daily_items.add( activity.findViewById(R.id.daily_item_bg_7) );
    }

    void show(int day, int second)
    {
        activity.runOnUiThread(() -> {
            int idx = day - 1;

            daily_main_layout.setVisibility(View.VISIBLE);

            for(int i = 0; i < daily_items.size(); i++) {
                ConstraintLayout parent = daily_items.get(i);

                ConstraintLayout label = (ConstraintLayout)parent.getChildAt(3);
                TextView labelText = (TextView)label.getChildAt(0);

                if(i == idx) {
                    parent.setAlpha(1.0f);

                    if(second == -1) {
                        parent.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#fbc02d")));

                        label.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#e57373")));
                        labelText.setText("Недоступно");
                    }
                    else if(second == 0) {
                        parent.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#fbc02d")));

                        label.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#66bb6a")));
                        labelText.setText("Забрать");
                    }
                    else {
                        parent.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#fbc02d")));

                        label.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#66bb6a")));
                        labelText.setText( DateUtils.formatElapsedTime(second) );
                    }

                    label.setOnClickListener(view -> {
                        click();
                    });
                }
                else if(i < idx) {
                    parent.setAlpha(0.4f);
                    parent.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#66bb6a")));

                    label.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#66bb6a")));
                    labelText.setText("Получено");
                }
                else {
                    parent.setAlpha(1.0f);
                    parent.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#9e9e9e")));

                    label.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#9e9e9e")));
                    labelText.setText( "Недоступно" );
                }
            }

        });

    }
}
