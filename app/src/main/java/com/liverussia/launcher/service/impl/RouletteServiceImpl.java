package com.liverussia.launcher.service.impl;

import android.app.Activity;

import com.liverussia.launcher.async.dto.response.ServerImagesResponseDto;
import com.liverussia.launcher.async.dto.response.SpinRouletteResponseDto;
import com.liverussia.launcher.async.rest.V3RestService;
import com.liverussia.launcher.async.rest.impl.V3RestServiceImpl;
import com.liverussia.launcher.service.RouletteService;

public class RouletteServiceImpl implements RouletteService {

    private final V3RestService v3RestService;
    private final Activity activity;

    public RouletteServiceImpl(Activity activity) {
        this.activity = activity;
        this.v3RestService = new V3RestServiceImpl(activity);
    }

    @Override
    public ServerImagesResponseDto getPossiblePrizes() {
        return v3RestService.sendGetPossiblePrizesRequest();
    }

    @Override
    public SpinRouletteResponseDto spinRoulette() {
        return v3RestService.sendSpinRouletteRequest();
    }
}
