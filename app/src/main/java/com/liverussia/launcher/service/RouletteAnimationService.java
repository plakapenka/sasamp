package com.liverussia.launcher.service;

import androidx.viewpager2.widget.ViewPager2;

import com.liverussia.launcher.domain.RouletteItem;
import com.liverussia.launcher.dto.response.PrizeInfoResponseDto;

public interface RouletteAnimationService {
    void setCurrentItem(final ViewPager2 pager, int item, long duration, PrizeInfoResponseDto prizeInfo);

    boolean isAnimationRunning();

    void cancelAnimation();
}
