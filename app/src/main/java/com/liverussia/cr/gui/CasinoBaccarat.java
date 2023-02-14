package com.liverussia.cr.gui;

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

import com.google.android.material.button.MaterialButton;
import com.liverussia.cr.R;
import com.liverussia.cr.core.Samp;
import com.nvidia.devtech.NvEventQueueActivity;

import org.checkerframework.checker.units.qual.A;

import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Random;

public class CasinoBaccarat
{
    boolean you_win;
    boolean bIsTempToggle = true;
    static final int ROUND_TIME = 25;
    static final int TIME_TO_BET = 10;
    static final int INVALID_CARD = -1;

    int curr_time = 0;
    int last_curr_bet = 0;
    int last_bet_type = BET_TYPE_NONE;
    int current_bet = 0;
    int current_bet_type = BET_TYPE_NONE;
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

    static final int BET_TYPE_NONE = 0;
    static final int BET_TYPE_RED = 1;
    static final int BET_TYPE_YELLOW = 2;
    static final int BET_TYPE_GREEN = 3;

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
    ConstraintLayout casino_bc_green_chip;
    ConstraintLayout casino_bc_red_card;
    ConstraintLayout casino_bc_yellow_card;
    ConstraintLayout casino_bc_timer_bg;
    TextView casino_bc_balance_value;
    MaterialButton casino_bc_exit_butt;
    ImageView casino_bc_cancel_button;
    ImageView casino_bc_repeat_button;
    ImageView casino_bc_off_sound_butt;

    // проценты
    TextView casino_bc_red_percent;
    TextView casino_bc_yellow_percent;
    TextView casino_bc_green_percent;

    boolean bOffSound = false;

    // last wins
    ArrayList <ImageView> casino_bc_last_wins = new ArrayList<>();

    ArrayList <Integer> last_bets = new ArrayList<>();

    int betsound = 0;

    native void sendAddBet(int sum, int bettype);
    native void init();
    native void exit();
    native void close();

    public CasinoBaccarat(Activity activity){
        init();
        this.activity = activity;

        casino_bc_off_sound_butt = activity.findViewById(R.id.casino_bc_off_sound_butt);
        casino_bc_off_sound_butt.setOnClickListener(view -> {
            activity.runOnUiThread(() -> {
                view.startAnimation( AnimationUtils.loadAnimation(activity, R.anim.button_click) );
                bOffSound = !bOffSound;
                if(bOffSound)
                    casino_bc_off_sound_butt.setImageTintList( ColorStateList.valueOf(Color.parseColor("#f44336")) );
                else
                    casino_bc_off_sound_butt.setImageTintList( ColorStateList.valueOf(Color.parseColor("#9e9e9e")) );
            });
        });

        casino_bc_cancel_button = activity.findViewById(R.id.casino_bc_cancel_button);
        casino_bc_cancel_button.setOnClickListener(view -> {
            if(last_bets.size() == 0) return;
            if(curr_time < TIME_TO_BET) {
                playNoMoreBets();
                return;
            }

            sendAddBet( last_bets.get( last_bets.size() - 1 ) * -1, BET_TYPE_NONE );

            last_bets.remove( last_bets.size() - 1 );
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
        });

        casino_bc_repeat_button = activity.findViewById(R.id.casino_bc_repeat_button);
        casino_bc_repeat_button.setOnClickListener(view -> {
            if(curr_time < TIME_TO_BET) {
                playNoMoreBets();
                return;
            }

            if(current_bet != 0) { // x2
                sendAddBet( current_bet, current_bet_type );
                last_bets.add( current_bet );
            }
            else {
                if( last_curr_bet <= 0 ) return;
                sendAddBet( last_curr_bet, last_bet_type );
                last_bets.add( last_curr_bet );
            }
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
        });

        // last wins
        casino_bc_last_wins.add( activity.findViewById(R.id.casino_bc_last_win_1) );
        casino_bc_last_wins.add( activity.findViewById(R.id.casino_bc_last_win_2) );
        casino_bc_last_wins.add( activity.findViewById(R.id.casino_bc_last_win_3) );
        casino_bc_last_wins.add( activity.findViewById(R.id.casino_bc_last_win_4) );
        casino_bc_last_wins.add( activity.findViewById(R.id.casino_bc_last_win_5) );
        casino_bc_last_wins.add( activity.findViewById(R.id.casino_bc_last_win_6) );
        casino_bc_last_wins.add( activity.findViewById(R.id.casino_bc_last_win_7) );
        casino_bc_last_wins.add( activity.findViewById(R.id.casino_bc_last_win_8) );
        casino_bc_last_wins.add( activity.findViewById(R.id.casino_bc_last_win_9) );

        casino_bc_balance_value = activity.findViewById(R.id.casino_bc_balance_value);

        casino_bc_red_percent = activity.findViewById(R.id.casino_bc_red_percent);
        casino_bc_yellow_percent = activity.findViewById(R.id.casino_bc_yellow_percent);
        casino_bc_green_percent = activity.findViewById(R.id.casino_bc_green_percent);

        casino_bc_exit_butt = activity.findViewById(R.id.casino_bc_exit_butt);
        casino_bc_exit_butt.setOnClickListener(view -> {
            exit();
        });
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
            if(curr_time < TIME_TO_BET) {
                playNoMoreBets();
                return;
            }

            sendAddBet( getChipValue(selected_chip_type), BET_TYPE_RED );

            last_bets.add( getChipValue(selected_chip_type) );

        });

        casino_bc_green_area = activity.findViewById(R.id.casino_bc_green_area);
        casino_bc_green_area.setOnClickListener(view -> {
            if(selected_chip_type == CHIP_TYPE_NONE) return;
            if(curr_time < TIME_TO_BET) {
                playNoMoreBets();
                return;
            }

            sendAddBet( getChipValue(selected_chip_type), BET_TYPE_GREEN );

            last_bets.add( getChipValue(selected_chip_type) );
        });

        casino_bc_yellow_area = activity.findViewById(R.id.casino_bc_yellow_area);
        casino_bc_yellow_area.setOnClickListener(view -> {
            if(selected_chip_type == CHIP_TYPE_NONE) return;
            if(curr_time < TIME_TO_BET) {
                playNoMoreBets();
                return;
            }

            sendAddBet( getChipValue(selected_chip_type), BET_TYPE_YELLOW );

            last_bets.add( getChipValue(selected_chip_type) );
        });

        casino_bc_red_chip = activity.findViewById(R.id.casino_bc_red_chip);
        casino_bc_yellow_chip = activity.findViewById(R.id.casino_bc_yellow_chip);
        casino_bc_green_chip = activity.findViewById(R.id.casino_bc_green_chip);

        betsound = Samp.soundPool.load(NvEventQueueActivity.getInstance(), R.raw.betchip, 0);
    }

    @SuppressLint({"DefaultLocale", "SetTextI18n"})
    void update(int red_card, int yellow_card, int totalBets, int totalRed, int totalYellow, int totalGreen, int time, int betType, int betSum, int winner, int balance)
    {
        curr_time = time;

        activity.runOnUiThread(() -> {
            TextView timetext = (TextView) casino_bc_timer_bg.getChildAt(0);
            if( bIsTempToggle ) casino_bc_main_layout.setVisibility(View.VISIBLE);
            casino_bc_balance_value.setText( String.format("%s", Samp.formatter.format(balance) ) );

            if(time == 4) {
                showWinner(winner);
            }

            if(time == ROUND_TIME) {
                if(!bOffSound) {
                    switch (new Random().nextInt(2)) {
                        case 0: {
                            NvEventQueueActivity.getInstance().PlaySound("http://files.liverussia.online/sounds/casino/go_bet_1.wav");
                            break;
                        }
                        default: {
                            NvEventQueueActivity.getInstance().PlaySound("http://files.liverussia.online/sounds/casino/go_bet_2.wav");
                            break;
                        }
                    }
                }
                reset();
            }

            if(time == TIME_TO_BET) {
                playNoMoreBets();
            }

            if(time > TIME_TO_BET )
                timetext.setText( String.format("%d", time - TIME_TO_BET) );
            else
                timetext.setText( "-" );

            // percents
            if(totalBets > 0)
            {
                casino_bc_red_percent.setText( String.format("%.0f%%", (float)totalRed / totalBets * 100f) );
                casino_bc_yellow_percent.setText( String.format("%.0f%%", (float)totalYellow / totalBets * 100f) );
                casino_bc_green_percent.setText( String.format("%.0f%%", (float)totalGreen / totalBets * 100f) );
            }


            if(red_card != 0){
                if(casino_bc_red_card.getVisibility() == View.GONE) {
                    updateRedCard(red_card);
                }
            }
            if(yellow_card != 0) {
                if (casino_bc_yellow_card.getVisibility() == View.GONE) {
                    updateYellowCard(yellow_card);
                }
            }

            // bets
            if(betSum != 0) {
                casino_bc_repeat_button.setImageResource(R.drawable.casino_bc_x2_button);
            }
            else {
                casino_bc_repeat_button.setImageResource(R.drawable.casino_bc_repeat_button);
            }

            current_bet_type = betType;

            if (betSum != 0) {
                if (current_bet != betSum) {
                    current_bet = betSum;
                    TextView chip_count;
                    if (betType == BET_TYPE_RED) {
                        chip_count = (TextView) casino_bc_red_chip.getChildAt(0);
                        if (casino_bc_red_chip.getVisibility() == View.GONE) {
                            casino_bc_red_chip.setVisibility(View.VISIBLE);
                            casino_bc_red_chip.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.popup_show_notification));
                        }
                        if(!bOffSound) Samp.soundPool.play(betsound, 0.2f, 0.2f, 1, 0, 1.0f);
                    }
                    else if (betType == BET_TYPE_YELLOW) {
                        chip_count = (TextView) casino_bc_yellow_chip.getChildAt(0);
                        if (casino_bc_yellow_chip.getVisibility() == View.GONE) {
                            casino_bc_yellow_chip.setVisibility(View.VISIBLE);
                            casino_bc_yellow_chip.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.popup_show_notification));
                        }
                        if(!bOffSound) Samp.soundPool.play(betsound, 0.2f, 0.2f, 1, 0, 1.0f);
                    }
                    else if (betType == BET_TYPE_GREEN) {
                        chip_count = (TextView) casino_bc_green_chip.getChildAt(0);
                        if (casino_bc_green_chip.getVisibility() == View.GONE) {
                            casino_bc_green_chip.setVisibility(View.VISIBLE);
                            casino_bc_green_chip.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.popup_show_notification));
                        }
                        if(!bOffSound) Samp.soundPool.play(betsound, 0.2f, 0.2f, 1, 0, 1.0f);
                    }
                    else return;

                    chip_count.setText(String.format("%d", betSum));
                }
            } else {
                if (casino_bc_yellow_chip.getVisibility() == View.VISIBLE || casino_bc_red_chip.getVisibility() == View.VISIBLE || casino_bc_green_chip.getVisibility() == View.VISIBLE) {
                    current_bet = betSum;
                    casino_bc_yellow_chip.setVisibility(View.GONE);
                    casino_bc_red_chip.setVisibility(View.GONE);
                    casino_bc_green_chip.setVisibility(View.GONE);
                }
            }

        });
    }

    void playNoMoreBets()
    {
        if(bOffSound) return;
        switch (new Random().nextInt(3)) {
            case 0:{
                NvEventQueueActivity.getInstance().PlaySound("http://files.liverussia.online/sounds/casino/no_bet_1.wav");
                break;
            }
            case 1:{
                NvEventQueueActivity.getInstance().PlaySound("http://files.liverussia.online/sounds/casino/no_bet_2.wav");
                break;
            }
            default: {
                NvEventQueueActivity.getInstance().PlaySound("http://files.liverussia.online/sounds/casino/no_bet_3.wav");
                break;
            }
        }
    }

    void reset()
    {
        if(!bOffSound) {
            if (you_win) {
                NvEventQueueActivity.getInstance().PlaySound("http://files.liverussia.online/sounds/casino/chip_win.mp3");
            } else {
                NvEventQueueActivity.getInstance().PlaySound("http://files.liverussia.online/sounds/casino/cards_clear.mp3");
            }
        }
        last_bets.clear();
        last_curr_bet = current_bet;
        last_bet_type = current_bet_type;

        current_bet = 0;
        current_bet_type = BET_TYPE_NONE;
        you_win = false;
        casino_bc_yellow_card.setVisibility(View.GONE);
        casino_bc_red_card.setVisibility(View.GONE);
        activity.findViewById(R.id.casino_bc_win_layout).setVisibility(View.GONE);
    }

    void tempToggle(boolean toggle) {
        activity.runOnUiThread(() -> {
            if(toggle)
                casino_bc_main_layout.setVisibility(View.VISIBLE);
            else
                casino_bc_main_layout.setVisibility(View.GONE);
        });
        bIsTempToggle = toggle;
    }

    String[] cards_value = {"0", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    String[] cards_suit_res = {"card_clubs", "card_diamonds", "card_hearts", "card_spades"};
    String[] card_color = {"#000000", "#F24E1E", "#F24E1E", "#000000"};

    void updateYellowCard(int cardNum) {
        if(!bOffSound) NvEventQueueActivity.getInstance().PlaySound("http://files.liverussia.online/sounds/casino/cards.mp3");

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
        if(!bOffSound) NvEventQueueActivity.getInstance().PlaySound("http://files.liverussia.online/sounds/casino/cards.mp3");

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

    void showWinner(int winner)
    {
        if(current_bet <= 0) return;

        activity.runOnUiThread( () -> {
            ConstraintLayout casino_bc_win_layout = activity.findViewById(R.id.casino_bc_win_layout);
            TextView casino_bc_win_caption = activity.findViewById(R.id.casino_bc_win_caption);
            TextView casino_bc_win_value = activity.findViewById(R.id.casino_bc_win_value);
            ImageView casino_bc_win_icon = activity.findViewById(R.id.casino_bc_win_icon);

            TextView casino_bc_win_text = activity.findViewById(R.id.casino_bc_win_text);
            View casino_bc_win_header = activity.findViewById(R.id.casino_bc_win_header);

            if(current_bet_type == winner) {
                if(!bOffSound) {
                    switch (new Random().nextInt(3)) {
                        case 0: {
                            NvEventQueueActivity.getInstance().PlaySound("http://files.liverussia.online/sounds/casino/win_1.wav");
                            break;
                        }
                        case 1: {
                            NvEventQueueActivity.getInstance().PlaySound("http://files.liverussia.online/sounds/casino/win_2.wav");
                            break;
                        }
                        default: {
                            NvEventQueueActivity.getInstance().PlaySound("http://files.liverussia.online/sounds/casino/win_3.wav");
                            break;
                        }
                    }
                }

                you_win = true;
                casino_bc_win_text.setText( "Вы выиграли" );

                if(current_bet_type != BET_TYPE_GREEN) {
                    casino_bc_win_value.setText( String.format("%d", current_bet * 2) );
                }
                else {
                    casino_bc_win_value.setText( String.format("%d", current_bet * 11) );
                }
                casino_bc_win_value.setVisibility(View.VISIBLE);
                casino_bc_win_icon.setVisibility(View.VISIBLE);
            }
            else {
                casino_bc_win_text.setText( "Вы проиграли" );
                casino_bc_win_value.setVisibility(View.GONE);
                casino_bc_win_icon.setVisibility(View.GONE);
            }

            switch (winner)
            {
                case BET_TYPE_RED: {
                    casino_bc_win_caption.setText("Black Casha");
                    casino_bc_win_header.setBackgroundTintList( ColorStateList.valueOf(Color.parseColor("#FF3131")) );
                    break;
                }
                case BET_TYPE_YELLOW: {
                    casino_bc_win_caption.setText("Live Russia");
                    casino_bc_win_header.setBackgroundTintList( ColorStateList.valueOf(Color.parseColor("#FFC223")) );
                    break;
                }
                case BET_TYPE_GREEN: {
                    casino_bc_win_caption.setText("Ничья");
                    casino_bc_win_header.setBackgroundTintList( ColorStateList.valueOf(Color.parseColor("#205537")) );
                    break;
                }
                default:
            }

            casino_bc_win_layout.setVisibility(View.VISIBLE);
        });

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

    void updateLastWins(int[] lastwins)
    {
        for(int i = 0; i < lastwins.length; i++)
        {
            if(lastwins[i] == BET_TYPE_RED) {
                casino_bc_last_wins.get(i).setImageTintList( ColorStateList.valueOf(Color.parseColor("#5C1C1D")) );
            }
            else if(lastwins[i] == BET_TYPE_YELLOW) {
                casino_bc_last_wins.get(i).setImageTintList( ColorStateList.valueOf(Color.parseColor("#9F8731")) );
            }
            else {
                casino_bc_last_wins.get(i).setImageTintList( ColorStateList.valueOf(Color.parseColor("#205537")) );
            }
        }
    }

    void hide() {
        if(!bOffSound) {
            switch (new Random().nextInt(2)) {
                case 0: {
                    NvEventQueueActivity.getInstance().PlaySound("http://files.liverussia.online/sounds/casino/exit_2.wav");
                    break;
                }
                default: {
                    NvEventQueueActivity.getInstance().PlaySound("http://files.liverussia.online/sounds/casino/exit_1.wav");
                    break;
                }
            }
        }
        activity.runOnUiThread( ()-> {
            reset();
            casino_bc_main_layout.setVisibility(View.GONE);
        });
    }

}
