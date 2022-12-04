package com.liverussia.launcher.async.rest;

import com.liverussia.launcher.async.dto.request.LoginRequestDto;
import com.liverussia.launcher.async.dto.request.RefreshTokenRequestDto;
import com.liverussia.launcher.async.dto.response.AuthenticationResponseDto;
import com.liverussia.launcher.async.dto.response.LoaderSliderInfoResponseDto;
import com.liverussia.launcher.async.dto.response.ServerImagesResponseDto;
import com.liverussia.launcher.async.dto.response.SpinRouletteResponseDto;
import com.liverussia.launcher.async.dto.response.UserInfoDto;

public interface V3RestService {
    AuthenticationResponseDto sendLoginRequest(LoginRequestDto loginRequestDto);

    AuthenticationResponseDto sendRefreshTokenRequest(RefreshTokenRequestDto refreshTokenRequestDto);

    UserInfoDto sendUpdateUserInfoRequest();

    ServerImagesResponseDto sendGetPossiblePrizesRequest();

    SpinRouletteResponseDto sendSpinRouletteRequest();

    ServerImagesResponseDto sendGetDonateServicesRequest();

    LoaderSliderInfoResponseDto sendGetLoaderSliderInfoRequest();
}
