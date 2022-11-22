package com.liverussia.cr.gui;

import android.app.Activity;
import android.graphics.Color;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.skydoves.progressview.ProgressView;

import java.util.ArrayList;
import java.util.Date;
import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;

public class MineGame1 {

    private ArrayList<ImageView> diamond_items = new ArrayList<>();
    private ArrayList<Integer> dimonds = new ArrayList<Integer>();
    private Activity activity;
    private ConstraintLayout mine_1_example;
    private ProgressView mine_1_progress;
    private TextView mine_1_score;
    private ConstraintLayout mine_1_main_layout;
    private View mine_1_exit_butt;
    private int score;
    public native void Init();
    public static native void ExitMineGame(int exittype);

    public MineGame1(Activity activity)
    {
        Init();

        this.activity = activity;

        mine_1_exit_butt = activity.findViewById(R.id.mine_1_exit_butt);
        mine_1_exit_butt.setOnClickListener(view -> {
            ExitMineGame(0);
            activity.runOnUiThread( () -> Utils.HideLayout(mine_1_main_layout, true) );
        });

        mine_1_main_layout = activity.findViewById(R.id.mine_1_main_layout);
        mine_1_main_layout.setVisibility(View.GONE);
        Utils.HideLayout(mine_1_main_layout, true);

        mine_1_example = activity.findViewById(R.id.mine_1_example);
        mine_1_progress = activity.findViewById(R.id.mine_1_progress);
        mine_1_score = activity.findViewById(R.id.mine_1_score);

        dimonds.add(R.drawable.mine_diamond_1);
        dimonds.add(R.drawable.mine_diamond_2);
        dimonds.add(R.drawable.mine_diamond_3);
        dimonds.add(R.drawable.mine_diamond_4);
        dimonds.add(R.drawable.mine_diamond_5);
        dimonds.add(R.drawable.mine_diamond_6);

        diamond_items.add(activity.findViewById(R.id.mine_1_item_1));
        diamond_items.add(activity.findViewById(R.id.mine_1_item_2));
        diamond_items.add(activity.findViewById(R.id.mine_1_item_3));
        diamond_items.add(activity.findViewById(R.id.mine_1_item_4));
        diamond_items.add(activity.findViewById(R.id.mine_1_item_5));
        diamond_items.add(activity.findViewById(R.id.mine_1_item_6));
        diamond_items.add(activity.findViewById(R.id.mine_1_item_7));
        diamond_items.add(activity.findViewById(R.id.mine_1_item_8));
        diamond_items.add(activity.findViewById(R.id.mine_1_item_9));
        diamond_items.add(activity.findViewById(R.id.mine_1_item_10));
        diamond_items.add(activity.findViewById(R.id.mine_1_item_11));
        diamond_items.add(activity.findViewById(R.id.mine_1_item_12));
        diamond_items.add(activity.findViewById(R.id.mine_1_item_13));
        diamond_items.add(activity.findViewById(R.id.mine_1_item_14));
        diamond_items.add(activity.findViewById(R.id.mine_1_item_15));
        diamond_items.add(activity.findViewById(R.id.mine_1_item_16));
        diamond_items.add(activity.findViewById(R.id.mine_1_item_17));
        diamond_items.add(activity.findViewById(R.id.mine_1_item_18));

        for(int i = 0; i < diamond_items.size(); i++){
            diamond_items.get(i).setOnClickListener(view -> {
                if(view.getTag() == mine_1_example.getChildAt(0).getTag())
                {// верно
                    this.score += 20;
                    mine_1_progress.setBorderColor(Color.parseColor("#66bb6a"));
                    mine_1_score.setTextColor(Color.parseColor("#66bb6a"));
                }
                else {
                    mine_1_progress.setBorderColor(Color.parseColor("#ef5350"));
                    mine_1_score.setTextColor(Color.parseColor("#ef5350"));
                    this.score -= 20;
                    if(this.score < 0) this.score = 0;
                }
                if(this.score >= 100) {
                    ExitMineGame(1);
                    activity.runOnUiThread( () -> Utils.HideLayout(mine_1_main_layout, true) );
                    return;
                }
                else {
                    TimerTask task = new TimerTask() {
                        public void run() {
                            activity.runOnUiThread(() -> {
                                mine_1_progress.setBorderColor(Color.parseColor("#ffffff"));
                                mine_1_score.setTextColor(Color.parseColor("#ffffff"));
                            });

                        }
                    };
                    Timer timer = new Timer("Timer");

                    timer.schedule(task, 1500L);
                }

                ReShuffle();
                mine_1_score.setText(String.format("00%d", score));
                mine_1_progress.setProgress(this.score);
            });
        }
    }
    public void Toggle(boolean toggle)
    {
        if(!toggle){
            Utils.HideLayout(mine_1_main_layout, true);
        }else {
            activity.runOnUiThread(() -> {
                this.score = 0;
                mine_1_progress.setProgress(0);
                ReShuffle();
                Utils.ShowLayout(mine_1_main_layout, true);
            });
        }
    }

    public void ReShuffle()
    {
        Random random = new Random();
        for(int i = 0; i < diamond_items.size(); i++)
        {
            int tmp = random.nextInt(dimonds.size());
            diamond_items.get(i).setImageResource(dimonds.get(tmp));
            diamond_items.get(i).setTag(tmp);
        }
        int tmp = random.nextInt(dimonds.size());
        ImageView example = (ImageView)mine_1_example.getChildAt(0);
        example.setImageResource(dimonds.get(tmp));
        example.setTag(tmp);

        //чтобы нужный элемент полюбому был среди рандомных
        int rnd_item = random.nextInt(diamond_items.size());
        diamond_items.get(rnd_item).setImageResource(dimonds.get(tmp));
        diamond_items.get(rnd_item).setTag(tmp);
    }
}
