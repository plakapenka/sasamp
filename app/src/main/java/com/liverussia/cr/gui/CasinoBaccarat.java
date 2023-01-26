package com.liverussia.cr.gui;

import static com.liverussia.cr.gui.CasinoBaccarat.BetsType.BET_TYPE_NONE;
import static com.liverussia.cr.gui.CasinoBaccarat.BetsType.BET_TYPE_RED;
import static com.liverussia.cr.gui.CasinoBaccarat.BetsType.BET_TYPE_YELLOW;
import static com.liverussia.cr.gui.CasinoBaccarat.ChipType.CHIP_TYPE_1;
import static com.liverussia.cr.gui.CasinoBaccarat.ChipType.CHIP_TYPE_100;
import static com.liverussia.cr.gui.CasinoBaccarat.ChipType.CHIP_TYPE_1000;
import static com.liverussia.cr.gui.CasinoBaccarat.ChipType.CHIP_TYPE_25;
import static com.liverussia.cr.gui.CasinoBaccarat.ChipType.CHIP_TYPE_5;
import static com.liverussia.cr.gui.CasinoBaccarat.ChipType.CHIP_TYPE_500;
import static com.liverussia.cr.gui.CasinoBaccarat.ChipType.CHIP_TYPE_NONE;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.util.Log;
import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;
import com.nvidia.devtech.NvEventQueueActivity;

import java.util.Random;

public class CasinoBaccarat {

    static final int INVALID_CARD = -1;
    int current_bet = 0;
    enum ChipType
    {
        CHIP_TYPE_NONE,
        CHIP_TYPE_1,
        CHIP_TYPE_5,
        CHIP_TYPE_25,
        CHIP_TYPE_100,
        CHIP_TYPE_500,
        CHIP_TYPE_1000
    }

    enum BetsType
    {
        BET_TYPE_NONE,
        BET_TYPE_RED,
        BET_TYPE_YELLOW,
        BET_TYPE_GREEN
    }

    ChipType selected_chip_type = CHIP_TYPE_NONE;

    ImageView casino_bc_chip_1;
    ImageView casino_bc_chip_5;
    ImageView casino_bc_chip_25;
    ImageView casino_bc_chip_100;
    ImageView casino_bc_chip_500;
    ImageView casino_bc_chip_1000;

    Activity activity;
    ConstraintLayout casino_bc_main_layout;
    ConstraintLayout casino_bc_red_area;
    ConstraintLayout casino_bc_green_area;
    ConstraintLayout casino_bc_yellow_area;
    ConstraintLayout casino_bc_red_chip;
    ConstraintLayout casino_bc_yellow_chip;
    ConstraintLayout casino_bc_red_card;
    ConstraintLayout casino_bc_yellow_card;
    ConstraintLayout casino_bc_timer_bg;

    int betsound = 0;

    native void sendAddBet(int sum, int bettype);
    native void init();

    public CasinoBaccarat(Activity activity){
        init();
        this.activity = activity;

        // ==== выделение фишек
        casino_bc_chip_1 = activity.findViewById(R.id.casino_bc_chip_1);
        casino_bc_chip_1.setOnClickListener(view -> {
            changeChipSelect(casino_bc_chip_1);
            selected_chip_type = CHIP_TYPE_1;
        });

        casino_bc_chip_5 = activity.findViewById(R.id.casino_bc_chip_5);
        casino_bc_chip_5.setOnClickListener(view -> {
            changeChipSelect(casino_bc_chip_5);
            selected_chip_type = CHIP_TYPE_5;
        });

        casino_bc_chip_25 = activity.findViewById(R.id.casino_bc_chip_25);
        casino_bc_chip_25.setOnClickListener(view -> {
            changeChipSelect(casino_bc_chip_25);
            selected_chip_type = CHIP_TYPE_25;
        });

        casino_bc_chip_100 = activity.findViewById(R.id.casino_bc_chip_100);
        casino_bc_chip_100.setOnClickListener(view -> {
            changeChipSelect(casino_bc_chip_100);
            selected_chip_type = CHIP_TYPE_100;
        });

        casino_bc_chip_500 = activity.findViewById(R.id.casino_bc_chip_500);
        casino_bc_chip_500.setOnClickListener(view -> {
            changeChipSelect(casino_bc_chip_500);
            selected_chip_type = CHIP_TYPE_500;
        });

        casino_bc_chip_1000 = activity.findViewById(R.id.casino_bc_chip_1000);
        casino_bc_chip_1000.setOnClickListener(view -> {
            changeChipSelect(casino_bc_chip_1000);
            selected_chip_type = CHIP_TYPE_1000;
        });

        // ==============
        casino_bc_timer_bg = activity.findViewById(R.id.casino_bc_timer_bg);
        casino_bc_red_card = activity.findViewById(R.id.casino_bc_red_card);
        casino_bc_yellow_card = activity.findViewById(R.id.casino_bc_yellow_card);

        casino_bc_main_layout = activity.findViewById(R.id.casino_bc_main_layout);
        casino_bc_main_layout.setVisibility(View.GONE);

        casino_bc_red_area = activity.findViewById(R.id.casino_bc_red_area);
        casino_bc_red_area.setOnClickListener(view -> {
            if(selected_chip_type == CHIP_TYPE_NONE) return;

            sendAddBet( getChipValue(selected_chip_type), BET_TYPE_RED.ordinal() );

        });

        casino_bc_green_area = activity.findViewById(R.id.casino_bc_green_area);
        casino_bc_green_area.setOnClickListener(view -> {

        });

        casino_bc_yellow_area = activity.findViewById(R.id.casino_bc_yellow_area);
        casino_bc_yellow_area.setOnClickListener(view -> {
            if(selected_chip_type == CHIP_TYPE_NONE) return;

            sendAddBet( getChipValue(selected_chip_type), BET_TYPE_YELLOW.ordinal() );
        });

        casino_bc_red_chip = activity.findViewById(R.id.casino_bc_red_chip);
        casino_bc_yellow_chip = activity.findViewById(R.id.casino_bc_yellow_chip);
    }

    void update(int red_card, int yellow_card, int totalBets, int totalRed, int totalYellow, int totalGreen, int time, int betType, int betSum)
    {
        Log.d("Sdf", "reciev = " + betType + "  " + betSum);
        activity.runOnUiThread(() -> {

            // main
            if(casino_bc_main_layout.getVisibility() == View.GONE) {
                casino_bc_main_layout.setVisibility(View.VISIBLE);
            }

            //cards
            if(red_card != 0) {
                if(casino_bc_red_card.getVisibility() == View.GONE) {
                    updateRedCard(red_card);
                }
            }
            else casino_bc_red_card.setVisibility(View.GONE);

            if(yellow_card != 0) {
                if(casino_bc_yellow_card.getVisibility() == View.GONE) {
                    updateYellowCard(yellow_card);
                }
            }
            else casino_bc_yellow_card.setVisibility(View.GONE);

            // bets
            if(betSum > 0) {
                if(current_bet != betSum) {
                    current_bet = betSum;
                    TextView chip_count;
                    if (betType == BET_TYPE_RED.ordinal()) {
                        chip_count = (TextView) casino_bc_red_chip.getChildAt(0);
                        if( casino_bc_red_chip.getVisibility() == View.GONE ) {
                            casino_bc_red_chip.setVisibility(View.VISIBLE);
                            casino_bc_red_chip.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.popup_show_notification));
                        }
                        NvEventQueueActivity.getInstance().soundPool.play(betsound, 0.2f, 0.2f, 1, 0, 1.0f);
                    }
                    else if (betType == BET_TYPE_YELLOW.ordinal()) {
                        chip_count = (TextView) casino_bc_yellow_chip.getChildAt(0);
                        if( casino_bc_yellow_chip.getVisibility() == View.GONE ) {
                            casino_bc_yellow_chip.setVisibility(View.VISIBLE);
                            casino_bc_yellow_chip.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.popup_show_notification));
                        }
                        NvEventQueueActivity.getInstance().soundPool.play(betsound, 0.2f, 0.2f, 1, 0, 1.0f);
                    } else return;

                    chip_count.setText(String.format("%d", betSum));
                }
            }
            else {
                if( casino_bc_yellow_chip.getVisibility() == View.VISIBLE || casino_bc_red_chip.getVisibility() == View.VISIBLE ) {
                    casino_bc_yellow_chip.setVisibility(View.GONE);
                    casino_bc_red_chip.setVisibility(View.GONE);
                }
            }

            // time
            TextView timetext = (TextView) casino_bc_timer_bg.getChildAt(0);
            if(time == -1) {
                timetext.setText("-");
            }
            else {
                timetext.setText(String.format("%d", time));
            }

        });
    }

    String[] cards_value = {"0", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    String[] cards_suit_res = {"0", "card_clubs", "card_diamonds", "card_hearts", "card_spades"};
    String[] card_color = {"#000000", "#F24E1E", "#F24E1E", "#000000"};

    void updateYellowCard(int cardNum) {
        int rnd = new Random().nextInt(4);
        TextView textTop = (TextView)casino_bc_yellow_card.getChildAt(0);
        TextView textBottom = (TextView)casino_bc_yellow_card.getChildAt(1);
        ImageView suit = (ImageView)casino_bc_yellow_card.getChildAt(2);

        textTop.setText(cards_value[cardNum]);
        textTop.setTextColor(Color.parseColor(card_color[rnd]));

        textBottom.setText(cards_value[cardNum]);
        textBottom.setTextColor(Color.parseColor(card_color[rnd]));

        suit.setImageResource(activity.getResources().getIdentifier(cards_suit_res[rnd], "drawable", activity.getPackageName()));

        casino_bc_yellow_card.setVisibility(View.VISIBLE);
        casino_bc_yellow_card.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.popup_show_notification));
    }

    void updateRedCard(int cardNum) {
        int rnd = new Random().nextInt(4);
        TextView textTop = (TextView)casino_bc_red_card.getChildAt(0);
        TextView textBottom = (TextView)casino_bc_red_card.getChildAt(1);
        ImageView suit = (ImageView)casino_bc_red_card.getChildAt(2);

        textTop.setText(cards_value[cardNum]);
        textTop.setTextColor(Color.parseColor(card_color[rnd]));

        textBottom.setText(cards_value[cardNum]);
        textBottom.setTextColor(Color.parseColor(card_color[rnd]));

        suit.setImageResource(activity.getResources().getIdentifier(cards_suit_res[rnd], "drawable", activity.getPackageName()));

        casino_bc_red_card.setVisibility(View.VISIBLE);
        casino_bc_red_card.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.popup_show_notification));
    }

    void changeChipSelect(ImageView selected) {
        casino_bc_chip_1.setBackgroundTintList(null);
        casino_bc_chip_5.setBackgroundTintList(null);
        casino_bc_chip_25.setBackgroundTintList(null);
        casino_bc_chip_100.setBackgroundTintList(null);
        casino_bc_chip_500.setBackgroundTintList(null);
        casino_bc_chip_1000.setBackgroundTintList(null);

        selected.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor("#b4004e")));
    }

    int getChipValue(ChipType chipType) {
        switch (chipType) {
            case CHIP_TYPE_1:       return 1;
            case CHIP_TYPE_5:       return 5;
            case CHIP_TYPE_25:      return 25;
            case CHIP_TYPE_100:     return 100;
            case CHIP_TYPE_500:     return 500;
            case CHIP_TYPE_1000:    return 1000;
            default:                return 0;
        }
    }

    void show(){
        activity.runOnUiThread(() -> {
            casino_bc_yellow_chip.setVisibility(View.GONE);
            casino_bc_red_chip.setVisibility(View.GONE);
            casino_bc_red_card.setVisibility(View.GONE);
            casino_bc_yellow_card.setVisibility(View.GONE);

            selected_chip_type = CHIP_TYPE_NONE;
        });
    }
}
