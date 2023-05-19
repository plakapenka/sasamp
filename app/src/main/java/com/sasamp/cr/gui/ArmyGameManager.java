package com.sasamp.cr.gui;

import android.app.Activity;
import android.text.Html;
import android.view.animation.AnimationUtils;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.sasamp.cr.R;
import com.sasamp.cr.gui.util.Utils;

public class ArmyGameManager {

    private FrameLayout br_army_layout;
    private ConstraintLayout army_game_main;
    private ProgressBar army_water_progress;
    private ProgressBar army_soap_progress;
    private ImageView army_mop_btn;
    private ImageView army_bucket_btn;
    private TextView army_water_procent;
    private TextView army_soap_procent;
    private TextView army_quantity_infotext;

    float armystate, armywaterstate, armysoapstate;

    native void onArmyGameClose();

    Runnable runnableoilgame = new Runnable() {
        @Override
        public void run() {
            while(armystate<2000) {
                armywaterstate -= 0.0001;
                armysoapstate -= 0.0001;
                if (armywaterstate < 0)
                {
                    armywaterstate = 0;
                }
                if (armywaterstate > 1000)
                {
                    armywaterstate = 1000;
                }
                if (armysoapstate < 0)
                {
                    armysoapstate = 0;
                }
                if (armysoapstate > 1000)
                {
                    armysoapstate = 1000;
                }
                army_water_progress.setProgress((int)armywaterstate);
                army_soap_progress.setProgress((int)armysoapstate);
                armystate = armywaterstate + armysoapstate;
            }
            Hide();
        }
    };

    public ArmyGameManager(Activity activity){
        br_army_layout = activity.findViewById(R.id.br_army_layout);
        army_water_progress = activity.findViewById(R.id.army_water_progress);
        army_soap_progress = activity.findViewById(R.id.army_soap_progress);
        army_mop_btn = activity.findViewById(R.id.army_mop_btn);
        army_bucket_btn = activity.findViewById(R.id.army_bucket_btn);
        army_water_procent = activity.findViewById(R.id.army_water_procent);
        army_soap_procent = activity.findViewById(R.id.army_soap_procent);
        army_game_main = activity.findViewById(R.id.army_game_main);
        army_quantity_infotext = activity.findViewById(R.id.army_quantity_infotext);

        army_mop_btn.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            armywaterstate += 100;
            army_water_progress.setProgress((int)armywaterstate);
            String stroilwaterprocent = String.format("%d%%", (int)armywaterstate/10);
            army_water_procent.setText(String.valueOf(stroilwaterprocent));
        });

        army_bucket_btn.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.button_click));
            armysoapstate += 100;
            army_soap_progress.setProgress((int)armysoapstate);
            String stroiloilprocent = String.format("%d%%", (int)armysoapstate/10);
            army_soap_procent.setText(String.valueOf(stroiloilprocent));
        });

        Utils.HideLayout(br_army_layout, false);
    }

    public void Show(int quantity) {
        armystate = 0;
        armywaterstate = 0;
        armysoapstate = 0;
        String ssarmy = String.format("%d/<font color='#FF3F33'>15</font>", quantity);
        army_quantity_infotext.setText(Html.fromHtml(String.valueOf(ssarmy)));
        Utils.ShowLayout(br_army_layout, true);
        Utils.ShowLayout(army_game_main, true);
        Thread thread = new Thread(runnableoilgame);
        thread.start();
    }

    public void Hide() {
        onArmyGameClose();
        Utils.HideLayout(army_game_main, true);
    }

    public void HideFull() {
        onArmyGameClose();
        Utils.HideLayout(br_army_layout, true);
    }
}