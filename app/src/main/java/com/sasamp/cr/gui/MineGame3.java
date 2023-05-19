package com.sasamp.cr.gui;

import static com.sasamp.cr.gui.Furniture_factory.getLocationView;

import android.app.Activity;
import android.graphics.Color;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.sasamp.cr.R;
import com.sasamp.cr.gui.util.Utils;
import com.skydoves.progressview.ProgressView;

import java.util.ArrayList;
import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;

public class MineGame3 {
    private Activity activity;
    private ConstraintLayout mine_3_main_layout;

    private int score;

    private ArrayList<Integer> dimonds = new ArrayList<Integer>();
    private ArrayList<ImageView> items = new ArrayList<>();
    private int dimonds_box_captions_color[] = new int[7];
    private String dimonds_box_captions[] = new String[7];
    private ArrayList<ConstraintLayout> box = new ArrayList<>();
    private ProgressView mine_3_progress;
    private TextView mine_3_score;
    private View mine_3_exit_butt;

    public native void Init();

    public MineGame3(Activity activity)
    {
        Init();

        mine_3_exit_butt = activity.findViewById(R.id.mine_3_exit_butt);
        mine_3_exit_butt.setOnClickListener(view -> {
            MineGame1.ExitMineGame(0);
            activity.runOnUiThread(() -> Utils.HideLayout(mine_3_main_layout, true));
        });

        mine_3_progress = activity.findViewById(R.id.mine_3_progress);
        mine_3_score = activity.findViewById(R.id.mine_3_score);

        dimonds_box_captions[2] = "Золото";
        dimonds_box_captions_color[2] = Color.parseColor("#fdd835");
        dimonds_box_captions[3] = "Кристалл";
        dimonds_box_captions_color[3] = Color.parseColor("#00e5ff");
        dimonds_box_captions[0] = "Кварц";
        dimonds_box_captions_color[0] = Color.parseColor("#f06292");
        dimonds_box_captions[4] = "Руда";
        dimonds_box_captions_color[4] = Color.parseColor("#8391AC");
        dimonds_box_captions[1] = "Медь";
        dimonds_box_captions_color[1] = Color.parseColor("#C1620A");
        dimonds_box_captions[5] = "Уголь";
        dimonds_box_captions_color[5] = Color.parseColor("#c7c7c7");

        box.add(activity.findViewById(R.id.mine_3_box_1));
        box.add(activity.findViewById(R.id.mine_3_box_2));
        box.add(activity.findViewById(R.id.mine_3_box_3));

        items.add(activity.findViewById(R.id.mine_3_item_1));
        items.add(activity.findViewById(R.id.mine_3_item_2));
        items.add(activity.findViewById(R.id.mine_3_item_3));

        for(int i = 0; i < items.size(); i++){
            items.get(i).setOnTouchListener(touchListener);
        }

      //  Init();

        this.activity = activity;

        mine_3_main_layout = activity.findViewById(R.id.mine_3_main_layout);

        mine_3_main_layout.setVisibility(View.GONE);
        Utils.HideLayout(mine_3_main_layout, true);

        dimonds.add(R.drawable.mine_diamond_1);
        dimonds.add(R.drawable.mine_diamond_2);
        dimonds.add(R.drawable.mine_diamond_3);
        dimonds.add(R.drawable.mine_diamond_4);
        dimonds.add(R.drawable.mine_diamond_5);
        dimonds.add(R.drawable.mine_diamond_6);


    }

    public void Toggle(boolean toggle)
    {
        activity.runOnUiThread(() -> {
            if (!toggle) {
                Utils.HideLayout(mine_3_main_layout, true);
            } else {

                ReShuffle();
                this.score = 0;
                mine_3_progress.setProgress(0);
                Utils.ShowLayout(mine_3_main_layout, true);

                mine_3_main_layout.post(() -> {
                    UpdateSizes();
                });

            }
        });
    }

    public void UpdateSizes()
    {
        //position
        ConstraintLayout.LayoutParams layoutParams;
        int screenwidth = activity.getResources().getDisplayMetrics().widthPixels;
        int screenheight = activity.getResources().getDisplayMetrics().heightPixels;

        for(int i = 0; i < items.size(); i++)
        {
            layoutParams = (ConstraintLayout.LayoutParams)items.get(i).getLayoutParams();

            //  items.get(i).setTop((int) mine_3_score.getY()- mine_3_score.getHeight());
            layoutParams.topMargin = (int) (screenheight-(items.get(i).getHeight()*1.8));

            switch (i){
                case 0:{
                    layoutParams.leftMargin = items.get(1).getLeft()-(items.get(0).getWidth()*2);
                    break;
                }
                case 1:{
                    layoutParams.leftMargin = screenwidth/2;
                    break;
                }
                case 2:{
                    layoutParams.leftMargin = items.get(1).getLeft()+(items.get(0).getWidth()*2);
                }
            }
            items.get(i).setLayoutParams(layoutParams);

        }
    }
    public void ReShuffle()
    {
        Random random = new Random();
        int rnd_cell = random.nextInt(dimonds.size());
        int rnd_diamond = random.nextInt(dimonds.size());
        for(int i = rnd_cell; i < rnd_cell+3; i++)
        {
            int j = i %3;

            items.get(j).setVisibility(View.VISIBLE);
            items.get(j).setImageResource(dimonds.get(rnd_diamond%dimonds.size()));
            items.get(j).setTag(rnd_diamond%dimonds.size());

            rnd_diamond++;
        }

        //

        rnd_cell = random.nextInt(dimonds.size());
        int d = 0;
        for(int i = rnd_cell; i < rnd_cell+3; i++)
        {
            int j = i %3;

            box.get(j).setTag(items.get(d).getTag());

            TextView boxcaption = (TextView) box.get(j).getChildAt(0);
            boxcaption.setText(dimonds_box_captions[(int)items.get(d).getTag()]);
            boxcaption.setTextColor(dimonds_box_captions_color[(int)items.get(d).getTag()]);
            boxcaption.setShadowLayer(10, 0, 0, dimonds_box_captions_color[(int)items.get(d).getTag()]);
            d++;
        }

    }
    private int getIndexByTag(View view)
    {
        for(int i = 0; i < box.size(); i++)
        {
            if(box.get(i).getTag() == view.getTag()){
                return i;
            }
        }
        return -1;
    }
    private int xDelta, yDelta;
    private View.OnTouchListener touchListener = new View.OnTouchListener() {
        @Override public boolean onTouch(View view, MotionEvent event) {
            final int x = (int) event.getRawX();
            final int y = (int) event.getRawY();

            ConstraintLayout.LayoutParams layoutParams = (ConstraintLayout.LayoutParams) view.getLayoutParams();
            int idx = getIndexByTag(view);
            int xbox = Math.abs( getLocationView(box.get(idx)).x );
            int ybox = Math.abs( getLocationView(box.get(idx)).y );

            switch (event.getAction() & MotionEvent.ACTION_MASK) {
                case MotionEvent.ACTION_DOWN: {
                    xDelta = x - layoutParams.leftMargin;
                    yDelta = y - layoutParams.topMargin;

                    break;
                }
                case MotionEvent.ACTION_UP:
                {
                    if(x > xbox && x < xbox+ box.get(idx).getWidth() && y > ybox && y < ybox + box.get(idx).getHeight()) {
                        activity.runOnUiThread(() -> {
                            view.setVisibility(View.GONE);
                            score += 100;
                            if(score >= 300){
                                MineGame1.ExitMineGame(1);
                                activity.runOnUiThread( () -> Utils.HideLayout(mine_3_main_layout, true) );
                                return;
                            }

                            mine_3_progress.setBorderColor(Color.parseColor("#66bb6a"));
                            mine_3_score.setTextColor(Color.parseColor("#66bb6a"));

                            mine_3_score.setText(String.format("00%d", score));
                            mine_3_progress.setProgress(score);
                        });

                        TimerTask task = new TimerTask() {
                            public void run() {
                                activity.runOnUiThread(() -> {
                                    mine_3_progress.setBorderColor(Color.parseColor("#ffffff"));
                                    mine_3_score.setTextColor(Color.parseColor("#ffffff"));
                                });

                            }
                        };
                        Timer timer = new Timer("Timer");

                        timer.schedule(task, 1500L);
                    }

                    box.get(idx).setScaleX((float) 1.0);
                    box.get(idx).setScaleY((float) 1.0);
                }

                case MotionEvent.ACTION_MOVE: {

                    layoutParams.leftMargin = x - xDelta;

                    if(x > xbox && x < xbox+ box.get(idx).getWidth() && y > ybox && y < ybox + box.get(idx).getHeight()){
                        box.get(idx).setScaleX((float) 1.1);
                        box.get(idx).setScaleY((float) 1.1);
                    }
                    else {
                        box.get(idx).setScaleX((float) 1.0);
                        box.get(idx).setScaleY((float) 1.0);
                    }
                    layoutParams.topMargin = y - yDelta;
                    layoutParams.rightMargin = 0;
                    layoutParams.bottomMargin = 0;
                    view.setLayoutParams(layoutParams);
                    break;
                }
            }
            return true;
        }
    };
}
