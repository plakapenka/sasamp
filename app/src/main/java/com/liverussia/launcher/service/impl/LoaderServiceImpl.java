package com.liverussia.launcher.service.impl;

import android.app.Activity;

import com.liverussia.launcher.async.dto.response.LoaderSliderInfoResponseDto;
import com.liverussia.launcher.async.rest.V3RestService;
import com.liverussia.launcher.async.rest.impl.V3RestServiceImpl;
import com.liverussia.launcher.service.LoaderService;

public class LoaderServiceImpl implements LoaderService {

    private final V3RestService v3RestService;
    private final Activity activity;

    public LoaderServiceImpl(Activity activity) {
        this.activity = activity;
        this.v3RestService = new V3RestServiceImpl(activity);
    }

    @Override
    public LoaderSliderInfoResponseDto getSliderInfo() {
        return v3RestService.sendGetLoaderSliderInfoRequest();
    }
}
