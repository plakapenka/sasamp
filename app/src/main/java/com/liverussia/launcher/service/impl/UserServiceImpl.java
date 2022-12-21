package com.liverussia.launcher.service.impl;

import android.app.Activity;

import com.liverussia.launcher.async.dto.response.UserInfoDto;
import com.liverussia.launcher.domain.enums.StorageElements;
import com.liverussia.launcher.service.UserService;
import com.liverussia.launcher.storage.Storage;

public class UserServiceImpl implements UserService {

    private final Activity activity;

    public UserServiceImpl(Activity activity) {
        this.activity = activity;
    }

    @Override
    public void updateUserInfoInStorage(UserInfoDto userInfoDto) {
        Storage.addProperty(StorageElements.AUTHENTICATED_NICKNAME.getValue(), userInfoDto.getUsername(), activity);
        Storage.addProperty(StorageElements.USER_BALANCE.getValue(), userInfoDto.getBalance(), activity);
        Storage.addProperty(StorageElements.AUTHENTICATED_SERVER.getValue(), userInfoDto.getServerName(), activity);
    }
}
