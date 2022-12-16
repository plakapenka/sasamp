package com.liverussia.launcher.ui.activity;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;

import com.akexorcist.roundcornerprogressbar.RoundCornerProgressBar;
import com.liverussia.cr.R;
import com.liverussia.launcher.ui.adapters.SliderStoriesAdapter;
import com.liverussia.launcher.utils.MainUtils;
import com.smarteist.autoimageslider.SliderAnimations;
import com.smarteist.autoimageslider.SliderView;

public class StoryActivity extends AppCompatActivity {

    private RoundCornerProgressBar progressStory;
    private SliderStoriesAdapter sliderStoriesAdapter;
    private SliderView sliderView;
    private CountDownTimer countDownTimer;
    private long progress;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_story);

        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);

        this.sliderView = (SliderView) findViewById(R.id.sliderView);
        this.progressStory = (RoundCornerProgressBar) findViewById(R.id.progressStory);
        ((ImageView) findViewById(R.id.closeStory)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                StoryActivity.this.lambda$onCreate$0$StoryActivity(view);
            }
        });
        SliderStoriesAdapter sliderStoriesAdapter = new SliderStoriesAdapter(this);
        this.sliderStoriesAdapter = sliderStoriesAdapter;
        this.sliderView.setSliderAdapter(sliderStoriesAdapter);
        this.sliderView.setSliderTransformAnimation(SliderAnimations.SIMPLETRANSFORMATION);
        this.sliderStoriesAdapter.addItems(MainUtils.NEWS);
        this.sliderView.setCurrentPageListener(i -> startTimer());
        int intExtra = getIntent().getIntExtra("position", 0);
        this.sliderView.getSliderPager().setCurrentItem(intExtra, false);
        this.sliderView.getPagerIndicator().setSelection(intExtra);
        this.progressStory.setProgress(0.0f);
        startTimer();
    }

    public void lambda$onCreate$0$StoryActivity(View view) {
        view.startAnimation(AnimationUtils.loadAnimation(this, R.anim.button_click));
        new Handler().postDelayed(() -> closeStory(), 200);
    }

    public void closeStory() {
        if(countDownTimer != null) {
            countDownTimer.cancel();
            countDownTimer = null;
        }
        progress = 0;
        startActivity(new Intent(getApplicationContext(), MainActivity.class));
    }

    private void startTimer() {
        if (countDownTimer != null) {
            countDownTimer.cancel();
            progress = 0;
            countDownTimer = null;
        }
        countDownTimer = new CountDownTimer(5000, 1) {
            @Override
            public void onTick(long j) {
                progress = 5000-j;
                progressStory.setProgress(progress);
            }
            @Override
            public void onFinish() {
                if(sliderView.getCurrentPagePosition() + 1 == sliderStoriesAdapter.getCount())
                    closeStory();
                else
                sliderView.setCurrentPagePosition(sliderView.getCurrentPagePosition() + 1);
            }
        }.start();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if(countDownTimer != null) {
            countDownTimer.cancel();
        }
        progress = 0;
        countDownTimer = null;
    }
}