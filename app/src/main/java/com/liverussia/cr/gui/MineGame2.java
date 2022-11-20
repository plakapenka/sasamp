package com.liverussia.cr.gui;

import android.app.Activity;
import android.graphics.Color;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.skydoves.progressview.ProgressView;

import java.util.ArrayList;
import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;

public class MineGame2 {
    private Activity activity;
    private ConstraintLayout mine_2_main_layout;
    private ConstraintLayout mine_2_example;
    private ImageView mine_2_conv_del;
    private ImageView mine_2_item_1;
    private ProgressView mine_2_progress;
    private TextView mine_2_score;
    private View mine_2_exit_butt;
    private int score;

    private ArrayList<ImageView> diamond_items = new ArrayList<>();
    private ArrayList<Integer> dimonds = new ArrayList<Integer>();

    public native void Init();

    public MineGame2(Activity activity)
    {
        Init();

        this.activity = activity;

        mine_2_example = activity.findViewById(R.id.mine_2_example);
        mine_2_main_layout = activity.findViewById(R.id.mine_2_main_layout);
        mine_2_conv_del = activity.findViewById(R.id.mine_2_conv_del);
        mine_2_progress = activity.findViewById(R.id.mine_2_progress);
        mine_2_score = activity.findViewById(R.id.mine_2_score);
        mine_2_exit_butt = activity.findViewById(R.id.mine_2_exit_butt);

        mine_2_item_1 = activity.findViewById(R.id.mine_2_item_1);

        mine_2_main_layout.setVisibility(View.GONE);
        Utils.HideLayout(mine_2_main_layout, true);

        diamond_items.add(activity.findViewById(R.id.mine_2_item_1));
        diamond_items.add(activity.findViewById(R.id.mine_2_item_2));
        diamond_items.add(activity.findViewById(R.id.mine_2_item_3));
        diamond_items.add(activity.findViewById(R.id.mine_2_item_4));
        diamond_items.add(activity.findViewById(R.id.mine_2_item_5));
        diamond_items.add(activity.findViewById(R.id.mine_2_item_6));
        diamond_items.add(activity.findViewById(R.id.mine_2_item_7));

        dimonds.add(R.drawable.mine_diamond_1);
        dimonds.add(R.drawable.mine_diamond_2);
        dimonds.add(R.drawable.mine_diamond_3);
        dimonds.add(R.drawable.mine_diamond_4);
        dimonds.add(R.drawable.mine_diamond_5);
        dimonds.add(R.drawable.mine_diamond_6);

        for(int i = 0; i < diamond_items.size(); i++){
            diamond_items.get(i).setOnClickListener(view -> {
                if(view.getTag() == mine_2_example.getChildAt(0).getTag())
                {// верно
                    this.score += 20;
                    mine_2_progress.setBorderColor(Color.parseColor("#66bb6a"));
                    mine_2_score.setTextColor(Color.parseColor("#66bb6a"));
                }
                else {
                    mine_2_progress.setBorderColor(Color.parseColor("#ef5350"));
                    mine_2_score.setTextColor(Color.parseColor("#ef5350"));
                    this.score -= 20;
                    if(this.score < 0) this.score = 0;
                }
                if(this.score >= 100) {
                    MineGame1.ExitMineGame(1);
                    activity.runOnUiThread( () -> Utils.HideLayout(mine_2_main_layout, true) );
                    return;
                }
                else {
                    TimerTask task = new TimerTask() {
                        public void run() {
                            activity.runOnUiThread(() -> {
                                mine_2_progress.setBorderColor(Color.parseColor("#ffffff"));
                                mine_2_score.setTextColor(Color.parseColor("#ffffff"));
                            });

                        }
                    };
                    Timer timer = new Timer("Timer");

                    timer.schedule(task, 1500L);
                }

                ReShuffle();
                mine_2_score.setText(String.format("00%d", score));
                mine_2_progress.setProgress(this.score);
            });
        }
    }

    public void Show()
    {
        activity.runOnUiThread(() -> {
            ReShuffle();
            this.score = 0;
            Utils.ShowLayout(mine_2_main_layout, true);
        });
//        Thread myThready = new Thread(new Runnable()
//        {
//            public void run() //Этот метод будет выполняться в побочном потоке
//            {
//                try {
//                    while (true) {
//                        ConstraintLayout.LayoutParams layoutParams = (ConstraintLayout.LayoutParams) mine_2_item_1.getLayoutParams();
//
//                        mine_2_item_1.setX(mine_2_item_1.getX()+1);
//
//                        mine_2_item_1.invalidate();
//
//                        sleep(1000);
//                        Log.d("dsa", "asdfsdf" + mine_2_item_1.getX());
//                    }
//                } catch (Exception ex) {
//                    ex.printStackTrace();
//                }
//            }
//        });
//        myThready.start();	//Запуск потока
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
        ImageView example = (ImageView)mine_2_example.getChildAt(0);
        example.setImageResource(dimonds.get(tmp));
        example.setTag(tmp);

        //чтобы нужный элемент полюбому был среди рандомных
        int rnd_item = random.nextInt(diamond_items.size());
        diamond_items.get(rnd_item).setImageResource(dimonds.get(tmp));
        diamond_items.get(rnd_item).setTag(tmp);
    }
}
