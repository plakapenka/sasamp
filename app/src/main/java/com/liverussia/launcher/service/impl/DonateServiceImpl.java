package com.liverussia.launcher.service.impl;

import android.app.Activity;

import com.liverussia.launcher.async.dto.response.ServerImagesResponseDto;
import com.liverussia.launcher.async.rest.V3RestService;
import com.liverussia.launcher.async.rest.impl.V3RestServiceImpl;
import com.liverussia.launcher.service.DonateService;

public class DonateServiceImpl implements DonateService {

    private final V3RestService v3RestService;
    private final Activity activity;

    public DonateServiceImpl(Activity activity) {
        this.activity = activity;
        this.v3RestService = new V3RestServiceImpl(activity);
    }

    @Override
    public ServerImagesResponseDto getDonateServices() {
        return v3RestService.sendGetDonateServicesRequest();
    }
}
