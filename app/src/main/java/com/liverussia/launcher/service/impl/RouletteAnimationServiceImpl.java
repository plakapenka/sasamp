package com.liverussia.launcher.service.impl;

import android.animation.Animator;
import android.animation.ValueAnimator;
import android.view.animation.AccelerateDecelerateInterpolator;

import androidx.viewpager2.widget.ViewPager2;

import com.liverussia.launcher.async.dto.response.PrizeInfoResponseDto;
import com.liverussia.launcher.ui.fragment.RouletteFragment;
import com.liverussia.launcher.service.RouletteAnimationService;

public class RouletteAnimationServiceImpl implements RouletteAnimationService {

    private final static int ZERO = 0;

    private final RouletteFragment rouletteFragment;
    private ValueAnimator animator;
    private int previousValue;

    public RouletteAnimationServiceImpl(RouletteFragment rouletteFragment) {
        this.rouletteFragment = rouletteFragment;
    }

    @Override
    public void setCurrentItem(final ViewPager2 rouletteUIPanel, int item, long duration, PrizeInfoResponseDto prizeInfo) {
        previousValue = ZERO;

        int currentItem = rouletteUIPanel.getCurrentItem();
        int pagePxWidth = rouletteUIPanel.getWidth();
        int pxToDrag = pagePxWidth * (item - currentItem);

        animator = ValueAnimator.ofInt(ZERO, pxToDrag);

        addUpdateListener(rouletteUIPanel);
        addAnimatorListener(rouletteUIPanel, prizeInfo);

        animator.setInterpolator(new AccelerateDecelerateInterpolator());
        animator.setDuration(duration);
        animator.start();
    }

    private void addUpdateListener(ViewPager2 rouletteUIPanel) {
        animator.addUpdateListener(animation -> {
            int currentValue = (int) animation.getAnimatedValue();
            float currentPxToDrag = (float) (currentValue - previousValue);
            rouletteUIPanel.fakeDragBy(-currentPxToDrag);
            previousValue = currentValue;
        });
    }

    private void addAnimatorListener(ViewPager2 rouletteUIPanel, PrizeInfoResponseDto prizeInfo) {
        animator.addListener(new Animator.AnimatorListener() {
            @Override
            public void onAnimationStart(Animator animation) {
                rouletteUIPanel.beginFakeDrag();
            }

            @Override
            public void onAnimationEnd(Animator animation) {
                rouletteFragment.openPrizeDialog(prizeInfo);
                rouletteUIPanel.endFakeDrag();
            }

            @Override
            public void onAnimationCancel(Animator animation) {}

            @Override
            public void onAnimationRepeat(Animator animation) {}
        });
    }

    @Override
    public boolean isAnimationRunning() {
        if (animator == null) {
            return false;
        }

        return animator.isRunning();
    }

    @Override
    public void cancelAnimation() {
        if (animator == null) {
            return;
        }

        if (animator.isRunning() || animator.isStarted()) {
            animator.cancel();
        }
    }
}
