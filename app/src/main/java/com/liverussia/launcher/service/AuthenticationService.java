package com.liverussia.launcher.service;

import com.liverussia.launcher.dto.request.LoginRequestDto;
import com.liverussia.launcher.dto.request.RefreshTokenRequestDto;
import com.liverussia.launcher.dto.response.AuthenticationResponseDto;
import com.liverussia.launcher.dto.response.UserInfoDto;

public interface AuthenticationService {
    AuthenticationResponseDto loginUser(LoginRequestDto loginRequestDto);

    boolean isAccessTokenHealth();

    boolean isRefreshTokenHealth();

    void saveUserInfoIntoStorage(AuthenticationResponseDto authenticationResponseDto);

    void logout();

    RefreshTokenRequestDto getRefreshTokenRequestDto();
}
