package com.liverussia.launcher.service.impl;

import android.app.Activity;

import com.liverussia.launcher.domain.Token;
import com.liverussia.launcher.async.dto.request.RefreshTokenRequestDto;
import com.liverussia.launcher.async.dto.response.AuthenticationResponseDto;
import com.liverussia.launcher.async.dto.response.UserInfoDto;
import com.liverussia.launcher.domain.enums.StorageElements;
import com.liverussia.launcher.service.AuthenticationService;
import com.liverussia.launcher.storage.Storage;
import com.liverussia.launcher.utils.DateTimeUtils;
import com.liverussia.launcher.utils.TokenUtils;

import org.apache.commons.lang3.StringUtils;

import org.threeten.bp.LocalDateTime;

import java.util.Optional;

public class AuthenticationServiceImpl implements AuthenticationService {

    private final Activity activity;
    private static final String BEARER_TOKEN_STARTS_STRING = "Bearer ";

    public AuthenticationServiceImpl(Activity activity) {
        this.activity = activity;
    }

    @Override
    public void logout() {
        Storage.deleteProperty(StorageElements.AUTHENTICATED_NICKNAME.getValue(), activity);
        Storage.deleteProperty(StorageElements.USER_BALANCE.getValue(), activity);
        Storage.deleteProperty(StorageElements.AUTHENTICATED_SERVER.getValue(), activity);
        Storage.deleteProperty(StorageElements.ACCESS_TOKEN.getValue(), activity);
        Storage.deleteProperty(StorageElements.REFRESH_TOKEN.getValue(), activity);
    }

    @Override
    public RefreshTokenRequestDto getRefreshTokenRequestDto() {
        RefreshTokenRequestDto refreshTokenRequestDto = new RefreshTokenRequestDto();
        String refreshToken = Storage.getProperty(StorageElements.REFRESH_TOKEN, activity);
        refreshTokenRequestDto.setRefreshToken(refreshToken);

        return refreshTokenRequestDto;
    }

    @Override
    public boolean isAccessTokenHealth() {
        String accessEncodedToken = Storage.getProperty(StorageElements.ACCESS_TOKEN, activity);
        return isTokenHealth(accessEncodedToken);
    }

    @Override
    public boolean isRefreshTokenHealth() {
        String refreshEncodedToken = Storage.getProperty(StorageElements.REFRESH_TOKEN, activity);
        return isTokenHealth(refreshEncodedToken);
    }

    private boolean isTokenHealth(String encodedToken) {
        if (StringUtils.isBlank(encodedToken)) {
            return false;
        }

        Token token = TokenUtils.decodeToken(encodedToken);

        if (token == null) {
            return false;
        }

        String tokenReceive = Storage.getProperty(StorageElements.TOKENS_RECEIVE_TIME, activity);

        if (StringUtils.isBlank(tokenReceive)) {
            return false;
        }

        LocalDateTime tokenReceiveTime = LocalDateTime.parse(tokenReceive);
        LocalDateTime expirationTime = TokenUtils.getLocalExpirationTime(token, tokenReceiveTime);

        LocalDateTime now = DateTimeUtils.getUTCNow();

        return now.isBefore(expirationTime);
    }

    @Override
    public void saveUserInfoIntoStorage(AuthenticationResponseDto authenticationResponseDto) {

        String balance = Optional.ofNullable(authenticationResponseDto.getUserInfo())
                .map(UserInfoDto::getBalance)
                .orElse(null);

        String username = Optional.ofNullable(authenticationResponseDto.getUserInfo())
                .map(UserInfoDto::getUsername)
                .orElse(null);

        String serverName = Optional.ofNullable(authenticationResponseDto.getUserInfo())
                .map(UserInfoDto::getServerName)
                .orElse(null);

        String accessToken = BEARER_TOKEN_STARTS_STRING.concat(authenticationResponseDto.getAccessToken());

        Storage.addProperty(StorageElements.AUTHENTICATED_NICKNAME, username, activity);
        Storage.addProperty(StorageElements.USER_BALANCE, balance, activity);
        Storage.addProperty(StorageElements.AUTHENTICATED_SERVER, serverName, activity);
        Storage.addProperty(StorageElements.ACCESS_TOKEN, accessToken, activity);
        Storage.addProperty(StorageElements.REFRESH_TOKEN, authenticationResponseDto.getRefreshToken(), activity);
        Storage.addProperty(StorageElements.TOKENS_RECEIVE_TIME, DateTimeUtils.getUTCNow().toString(), activity);
    }


}
