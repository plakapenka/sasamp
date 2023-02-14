package com.liverussia.cr.gui;

import android.app.Activity;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.google.android.material.button.MaterialButton;
import com.liverussia.cr.R;
import com.liverussia.cr.core.Samp;
import com.skydoves.progressview.ProgressView;

import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.Locale;

public class TechIspect {
    Activity activity;

    ConstraintLayout tech_inspect_main_layout;
    TextView tech_inspect_veh_name_text;
    TextView tech_inspect_price_text;

    TextView tech_inspect_generator_percent;
    ProgressView tech_inspect_generator_progress;

    TextView tech_inspect_candles_percent;
    ProgressView tech_inspect_candles_progress;

    TextView tech_inspect_brake_percent;
    ProgressView tech_inspect_brake_progress;

    TextView tech_inspect_starter_percent;
    ProgressView tech_inspect_starter_progress;

    TextView tech_inspect_nozzles_percent;
    ProgressView tech_inspect_nozzles_progress;

    MaterialButton tech_inspect_buy_butt;
    View tech_inspect_exit_butt;
    ImageView tech_inspect_veh_img;

    native void sendClick();
    native void init();
    native void exit();

    public TechIspect(Activity activity)
    {
        init();

        this.activity = activity;

        tech_inspect_veh_img = activity.findViewById(R.id.tech_inspect_veh_img);

        tech_inspect_exit_butt = activity.findViewById(R.id.tech_inspect_exit_butt);
        tech_inspect_exit_butt.setOnClickListener(view -> {
            tech_inspect_main_layout.setVisibility(View.GONE);
            exit();
        });

        tech_inspect_buy_butt = activity.findViewById(R.id.tech_inspect_buy_butt);
        tech_inspect_buy_butt.setOnClickListener(view -> {
            tech_inspect_main_layout.setVisibility(View.GONE);
            sendClick();
        });

        tech_inspect_main_layout = activity.findViewById(R.id.tech_inspect_main_layout);
        tech_inspect_main_layout.setVisibility(View.GONE);

        tech_inspect_veh_name_text = activity.findViewById(R.id.tech_inspect_veh_name_text);
        tech_inspect_price_text = activity.findViewById(R.id.tech_inspect_price_text);

        tech_inspect_generator_percent = activity.findViewById(R.id.tech_inspect_generator_percent);
        tech_inspect_generator_progress = activity.findViewById(R.id.tech_inspect_generator_progress);

        tech_inspect_candles_percent = activity.findViewById(R.id.tech_inspect_candles_percent);
        tech_inspect_candles_progress = activity.findViewById(R.id.tech_inspect_candles_progress);

        tech_inspect_brake_percent = activity.findViewById(R.id.tech_inspect_brake_percent);
        tech_inspect_brake_progress = activity.findViewById(R.id.tech_inspect_brake_progress);

        tech_inspect_starter_percent = activity.findViewById(R.id.tech_inspect_starter_percent);
        tech_inspect_starter_progress = activity.findViewById(R.id.tech_inspect_starter_progress);

        tech_inspect_nozzles_percent = activity.findViewById(R.id.tech_inspect_nozzles_percent);
        tech_inspect_nozzles_progress = activity.findViewById(R.id.tech_inspect_nozzles_progress);
    }

    void show(String name, int gen, int candl, int brake, int starter, int nozzles, int price, int carid)
    {
        activity.runOnUiThread(() -> {
            int imgres = 0;

            imgres = activity.getResources().getIdentifier("auc_veh_" + carid, "drawable", activity.getPackageName());
            tech_inspect_veh_img.setImageResource(imgres);

            tech_inspect_veh_name_text.setText(name);

            tech_inspect_generator_percent.setText( String.format("%d %%", 100 - gen) );
            tech_inspect_generator_progress.setProgress(100 - gen);

            tech_inspect_candles_percent.setText( String.format("%d %%", 100 - candl) );
            tech_inspect_candles_progress.setProgress(100 - candl);

            tech_inspect_brake_percent.setText( String.format("%d %%", 100 - brake) );
            tech_inspect_brake_progress.setProgress(100 - brake);

            tech_inspect_starter_percent.setText( String.format("%d %%", 100 - starter) );
            tech_inspect_starter_progress.setProgress(100 - starter);

            tech_inspect_nozzles_percent.setText( String.format("%d %%", 100 - nozzles) );
            tech_inspect_nozzles_progress.setProgress(100 - nozzles);

            tech_inspect_price_text.setText( String.format("Госпошлина %s рублей", Samp.formatter.format(price)) );

            tech_inspect_main_layout.setVisibility(View.VISIBLE);
        });
    }

}
