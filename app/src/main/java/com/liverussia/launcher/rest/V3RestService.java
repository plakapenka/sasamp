package com.liverussia.launcher.rest;

import com.liverussia.launcher.dto.request.LoginRequestDto;
import com.liverussia.launcher.dto.request.RefreshTokenRequestDto;
import com.liverussia.launcher.dto.response.AuthenticationResponseDto;
import com.liverussia.launcher.dto.response.LoaderSliderInfoResponseDto;
import com.liverussia.launcher.dto.response.ServerImagesResponseDto;
import com.liverussia.launcher.dto.response.SpinRouletteResponseDto;
import com.liverussia.launcher.dto.response.UserInfoDto;

public interface V3RestService {
    AuthenticationResponseDto sendLoginRequest(LoginRequestDto loginRequestDto);

    AuthenticationResponseDto sendRefreshTokenRequest(RefreshTokenRequestDto refreshTokenRequestDto);

    UserInfoDto sendUpdateUserInfoRequest();

    ServerImagesResponseDto sendGetPossiblePrizesRequest();

    SpinRouletteResponseDto sendSpinRouletteRequest();

    ServerImagesResponseDto sendGetDonateServicesRequest();

    LoaderSliderInfoResponseDto sendGetLoaderSliderInfoRequest();
}
