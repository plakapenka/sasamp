package com.liverussia.launcher.service.impl;

import android.app.Activity;

import androidx.appcompat.app.AppCompatActivity;

import com.liverussia.launcher.dto.response.UserInfoDto;
import com.liverussia.launcher.enums.StorageElements;
import com.liverussia.launcher.rest.V3RestService;
import com.liverussia.launcher.rest.impl.V3RestServiceImpl;
import com.liverussia.launcher.service.UserService;
import com.liverussia.launcher.storage.Storage;

public class UserServiceImpl implements UserService {

    private final V3RestService v3RestService;
    private final Activity activity;

    public UserServiceImpl(Activity activity) {
        this.activity = activity;
        this.v3RestService = new V3RestServiceImpl(activity);
    }

    @Override
    public void updateUserInfoInStorage(UserInfoDto userInfoDto) {
        Storage.addProperty(StorageElements.AUTHENTICATED_NICKNAME.getValue(), userInfoDto.getUsername(), activity);
        Storage.addProperty(StorageElements.USER_BALANCE.getValue(), userInfoDto.getBalance(), activity);
        Storage.addProperty(StorageElements.AUTHENTICATED_SERVER.getValue(), userInfoDto.getServerName(), activity);
    }
}
