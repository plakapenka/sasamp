package com.liverussia.launcher.service;

import com.liverussia.launcher.async.dto.request.LoginRequestDto;
import com.liverussia.launcher.async.dto.request.RefreshTokenRequestDto;
import com.liverussia.launcher.async.dto.response.AuthenticationResponseDto;

public interface AuthenticationService {

    boolean isAccessTokenHealth();

    boolean isRefreshTokenHealth();

    void saveUserInfoIntoStorage(AuthenticationResponseDto authenticationResponseDto);

    void logout();

    RefreshTokenRequestDto getRefreshTokenRequestDto();
}
