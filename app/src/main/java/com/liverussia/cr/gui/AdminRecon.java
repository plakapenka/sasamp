package com.liverussia.cr.gui;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.view.View;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.google.android.material.button.MaterialButton;
import com.liverussia.cr.R;

public class AdminRecon {
    private static final int EXIT_BUTTON = 0;
    private static final int STATS_BUTTON = 1;
    private static final int FREEZE_BUTTON = 2;
    private static final int UNFREEZE_BUTTON = 3;
    private static final int SPAWN_BUTTON = 4;
    private static final int SLAP_BUTTON = 5;
    private static final int REFRESH_BUTTON = 6;
    private static final int MUTE_BUTTON = 7;
    private static final int JAIL_BUTTON = 8;
    private static final int KICK_BUTTON = 9;
    private static final int BAN_BUTTON = 10;
    private static final int WARN_BUTTON = 11;
    private static final int NEXT_BUTTON = 12;
    private static final int PREV_BUTTON = 13;

    Activity activity;
    TextView re_nickname_text;
    TextView re_id_text;
    ConstraintLayout re_main_layout;
    MaterialButton re_exit_butt;
    ConstraintLayout re_stats_button;
    ConstraintLayout re_freeze_butt;
    ConstraintLayout re_unfreeze_butt;
    ConstraintLayout re_spawn_butt;
    ConstraintLayout re_slap_butt;
    MaterialButton re_refresh_button;
    ConstraintLayout re_mute_button;
    ConstraintLayout re_jail_butt;
    ConstraintLayout re_kick_butt;
    ConstraintLayout re_ban_butt;
    ConstraintLayout re_warn_butt;
    ConstraintLayout re_prev_button;
    ConstraintLayout re_next_button;

    public native void init();
    public native void clickButton(int buttonID);

    public AdminRecon(Activity activity){
        init();

        this.activity = activity;
        re_main_layout = activity.findViewById(R.id.re_main_layout);
        re_main_layout.setVisibility(View.GONE);

        re_nickname_text = activity.findViewById(R.id.re_nickname_text);
        re_id_text = activity.findViewById(R.id.re_id_text);

        re_exit_butt = activity.findViewById(R.id.re_exit_butt);
        re_exit_butt.setOnClickListener(view -> {
            clickButton(EXIT_BUTTON);
        });

        re_stats_button = activity.findViewById(R.id.re_stats_button);
        re_stats_button.setOnClickListener(view -> {
            clickButton(STATS_BUTTON);
        });

        re_freeze_butt = activity.findViewById(R.id.re_freeze_butt);
        re_freeze_butt.setOnClickListener(view -> {
            clickButton(FREEZE_BUTTON);
        });

        re_unfreeze_butt = activity.findViewById(R.id.re_unfreeze_butt);
        re_unfreeze_butt.setOnClickListener(view -> {
            clickButton(UNFREEZE_BUTTON);
        });

        re_spawn_butt = activity.findViewById(R.id.re_spawn_butt);
        re_spawn_butt.setOnClickListener(view -> {
            clickButton(SPAWN_BUTTON);
        });

        re_slap_butt = activity.findViewById(R.id.re_slap_butt);
        re_slap_butt.setOnClickListener(view -> {
            clickButton(SLAP_BUTTON);
        });

        re_refresh_button = activity.findViewById(R.id.re_refresh_button);
        re_refresh_button.setOnClickListener(view -> {
            clickButton(REFRESH_BUTTON);
        });

        re_mute_button = activity.findViewById(R.id.re_mute_button);
        re_mute_button.setOnClickListener(view -> {
            clickButton(MUTE_BUTTON);
        });

        re_jail_butt = activity.findViewById(R.id.re_jail_butt);
        re_jail_butt.setOnClickListener(view -> {
            clickButton(JAIL_BUTTON);
        });

        re_kick_butt = activity.findViewById(R.id.re_kick_butt);
        re_kick_butt.setOnClickListener(view -> {
            clickButton(KICK_BUTTON);
        });

        re_ban_butt = activity.findViewById(R.id.re_ban_butt);
        re_ban_butt.setOnClickListener(view -> {
            clickButton(BAN_BUTTON);
        });

        re_warn_butt = activity.findViewById(R.id.re_warn_butt);
        re_warn_butt.setOnClickListener(view -> {
            clickButton(WARN_BUTTON);
        });

        re_prev_button = activity.findViewById(R.id.re_prev_button);
        re_prev_button.setOnClickListener(view -> {
            clickButton(PREV_BUTTON);
        });

        re_next_button = activity.findViewById(R.id.re_next_button);
        re_next_button.setOnClickListener(view -> {
            clickButton(NEXT_BUTTON);
        });
    }

    @SuppressLint("DefaultLocale")
    void show(String nick, int id){
        activity.runOnUiThread(() -> {
            re_nickname_text.setText(nick);
            re_id_text.setText(String.format("%d", id));

            re_main_layout.setVisibility(View.VISIBLE);
        });
    }

    void hide(){
        activity.runOnUiThread(() -> {
            re_main_layout.setVisibility(View.GONE);
        });
    }
}
