package com.liverussia.cr.gui;

import android.app.Activity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.liverussia.cr.R;
import com.nvidia.devtech.NvEventQueueActivity;

import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.Locale;

public class CasinoDice {
    Activity activity;
    ConstraintLayout casino_dice_main_layout;
    TextView casino_dice_table_num;
    TextView dice_text_bet;
    TextView casino_dice_text_bank;
    TextView dice_text_balance;
    TextView dice_text_player_name1;
    TextView dice_text_player_ready1;
    TextView dice_text_player_name2;
    TextView dice_text_player_ready2;
    TextView dice_text_player_name3;
    TextView dice_text_player_ready3;
    TextView dice_text_player_name4;
    TextView dice_text_player_ready4;
    TextView dice_text_player_name5;
    TextView dice_text_player_ready5;
    Button dice_butt_bet;
    Button dice_butt_dice;
    Button dice_butt_exit;
    DecimalFormat num_format;
    ConstraintLayout dialog_layout;

    private final int CASINO_DICE_BUTTON_BET = 0;
    private final int CASINO_DICE_BUTTON_DICE = 1;
    private final int CASINO_DICE_BUTTON_EXIT = 2;

    public native void CasinoInit();
    public CasinoDice(Activity aactivity)
    {
        CasinoInit();

        DecimalFormatSymbols otherSymbols = new DecimalFormatSymbols(Locale.getDefault());
        otherSymbols.setGroupingSeparator('.');
        num_format = new DecimalFormat("###,###.###", otherSymbols);

        activity = aactivity;

        casino_dice_main_layout = activity.findViewById(R.id.casino_dice_main_layout);
        casino_dice_main_layout.setVisibility(View.GONE);
        casino_dice_table_num = activity.findViewById(R.id.casino_dice_table_num);
        dice_text_bet = activity.findViewById(R.id.dice_text_bet);
        casino_dice_text_bank = activity.findViewById(R.id.casino_dice_text_bank);
        dice_text_balance = activity.findViewById(R.id.dice_text_balance);

        dice_text_player_name1 = activity.findViewById(R.id.dice_text_player_name1);
        dice_text_player_ready1 = activity.findViewById(R.id.dice_text_player_ready1);

        dice_text_player_name2 = activity.findViewById(R.id.dice_text_player_name2);
        dice_text_player_ready2 = activity.findViewById(R.id.dice_text_player_ready2);

        dice_text_player_name3 = activity.findViewById(R.id.dice_text_player_name3);
        dice_text_player_ready3 = activity.findViewById(R.id.dice_text_player_ready3);

        dice_text_player_name4 = activity.findViewById(R.id.dice_text_player_name4);
        dice_text_player_ready4 = activity.findViewById(R.id.dice_text_player_ready4);

        dice_text_player_name5 = activity.findViewById(R.id.dice_text_player_name5);
        dice_text_player_ready5 = activity.findViewById(R.id.dice_text_player_ready5);

        dice_butt_bet = activity.findViewById(R.id.dice_butt_bet);
        dice_butt_dice = activity.findViewById(R.id.dice_butt_dice);
        dice_butt_exit = activity.findViewById(R.id.dice_butt_exit);

        dice_butt_bet.setOnClickListener(view -> {
            NvEventQueueActivity.getInstance().SendCasinoButt(CASINO_DICE_BUTTON_BET);
        });
        dice_butt_dice.setOnClickListener(view -> {
            NvEventQueueActivity.getInstance().SendCasinoButt(CASINO_DICE_BUTTON_DICE);
        });
        dice_butt_exit.setOnClickListener(view -> {
            NvEventQueueActivity.getInstance().SendCasinoButt(CASINO_DICE_BUTTON_EXIT);
        });
    }

    public void TempToggle(boolean toggle)
    {
        if(toggle)
        {
            casino_dice_main_layout.setVisibility(View.VISIBLE);
        }
        else
        {
            casino_dice_main_layout.setVisibility(View.GONE);
        }
    }
    public void Toggle(boolean show, int tableID, int tableBet, int tableBank, int money, String player1name, int player1stat, String player2name, int player2stat, String player3name, int player3stat, String player4name, int player4stat, String player5name, int player5stat)
    {
//        if(!show)
//        {
//            casino_dice_main_layout.setVisibility(View.GONE);
//            return;
//        }
//        casino_dice_table_num.setText("Стол "+tableID);
//        dice_text_bet.setText(num_format.format(tableBet)+" руб.");
//        casino_dice_text_bank.setText(num_format.format(tableBank)+" руб.");
//        dice_text_balance.setText(num_format.format(money)+" руб.");
//
//        dice_text_player_name1.setText(player1name);
//        if(player1stat == 0)
//        {
//            dice_text_player_ready1.setText("--");
//        }
//        else if(player1stat == 1)
//        {
//            dice_text_player_ready1.setText("++");
//        }
//        else
//        {
//            dice_text_player_ready1.setText(String.format("%d",player1stat) );
//        }
//        dice_text_player_name2.setText(player2name);
//        if(player2stat == 0)
//        {
//            dice_text_player_ready2.setText("--");
//        }
//        else if(player2stat == 1)
//        {
//            dice_text_player_ready2.setText("++");
//        }
//        else
//        {
//            dice_text_player_ready2.setText(String.format("%d",player2stat));
//        }
//        dice_text_player_name3.setText(player3name);
//        if(player3stat == 0)
//        {
//            dice_text_player_ready3.setText("--");
//        }
//        else if(player3stat == 1)
//        {
//            dice_text_player_ready3.setText("++");
//        }
//        else
//        {
//            dice_text_player_ready3.setText(String.format("%d",player3stat));
//        }
//        dice_text_player_name4.setText(player4name);
//        if(player4stat == 0)
//        {
//            dice_text_player_ready4.setText("--");
//        }
//        else if(player4stat == 1)
//        {
//            dice_text_player_ready4.setText("++");
//        }
//        else
//        {
//            dice_text_player_ready4.setText(String.format("%d",player4stat));
//        }
//        dice_text_player_name5.setText(player5name);
//        if(player5stat == 0)
//        {
//            dice_text_player_ready5.setText("--");
//        }
//        else if(player5stat == 1)
//        {
//            dice_text_player_ready5.setText("++");
//        }
//        else
//        {
//            dice_text_player_ready5.setText(String.format("%d",player5stat));
//        }
//        casino_dice_main_layout.setVisibility(View.VISIBLE);
        Log.d("sadf", "Hello from cpp2");
    }
    public void HelloWorld()
    {
        Log.d("sadf", "Hello from cpp");
    }
}


