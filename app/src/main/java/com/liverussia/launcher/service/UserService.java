package com.liverussia.launcher.service;

import com.liverussia.launcher.dto.response.UserInfoDto;

public interface UserService {
    void updateUserInfoInStorage(UserInfoDto userInfoDto);
}
