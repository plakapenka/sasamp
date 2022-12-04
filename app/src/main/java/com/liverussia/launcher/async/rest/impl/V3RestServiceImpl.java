package com.liverussia.launcher.async.rest.impl;

import android.app.Activity;


import com.liverussia.launcher.async.dto.request.LoginRequestDto;
import com.liverussia.launcher.async.dto.request.RefreshTokenRequestDto;
import com.liverussia.launcher.async.dto.response.AuthenticationResponseDto;
import com.liverussia.launcher.async.dto.response.LoaderSliderInfoResponseDto;
import com.liverussia.launcher.async.dto.response.ServerImagesResponseDto;
import com.liverussia.launcher.async.dto.response.SpinRouletteResponseDto;
import com.liverussia.launcher.async.dto.response.UserInfoDto;
import com.liverussia.launcher.domain.enums.StorageElements;
import com.liverussia.launcher.error.handler.RestErrorHandler;
import com.liverussia.launcher.async.rest.V3RestService;
import com.liverussia.launcher.storage.Storage;

import org.springframework.http.HttpEntity;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.http.ResponseEntity;
import org.springframework.http.converter.json.MappingJackson2HttpMessageConverter;
import org.springframework.web.client.RestTemplate;

import java.net.URI;

import static com.liverussia.launcher.config.Config.LAUNCHER_SERVER_GET_DONATE_SERVICES_URL;
import static com.liverussia.launcher.config.Config.LAUNCHER_SERVER_GET_LOADER_SLIDER_INFO_SERVICES_URL;
import static com.liverussia.launcher.config.Config.LAUNCHER_SERVER_GET_POSSIBLE_PRIZES_URL;
import static com.liverussia.launcher.config.Config.LAUNCHER_SERVER_LOGIN_URL;
import static com.liverussia.launcher.config.Config.LAUNCHER_SERVER_REFRESH_TOKEN_URL;
import static com.liverussia.launcher.config.Config.LAUNCHER_SERVER_SPIN_ROULETTE_URL;
import static com.liverussia.launcher.config.Config.LAUNCHER_SERVER_UPDATE_USER_INFO_URL;
import static com.liverussia.launcher.config.Config.LAUNCHER_SERVER_URI;

public class V3RestServiceImpl implements V3RestService {

    private final static String AUTHORIZATION_HEADER = "Authorization";

    private final RestTemplate restTemplate;
    private final Activity activity;

    public V3RestServiceImpl(Activity activity) {
        this.activity = activity;
    }

    {
        restTemplate = new RestTemplate();
        restTemplate.getMessageConverters().add(new MappingJackson2HttpMessageConverter());
        restTemplate.setErrorHandler(new RestErrorHandler());
    }


    @Override
    public AuthenticationResponseDto sendLoginRequest(LoginRequestDto requestDto) {
        URI url = URI.create(LAUNCHER_SERVER_URI + LAUNCHER_SERVER_LOGIN_URL);
        return sendRequest(url, requestDto, HttpMethod.POST, AuthenticationResponseDto.class);
    }

    @Override
    public AuthenticationResponseDto sendRefreshTokenRequest(RefreshTokenRequestDto requestDto) {
        URI url = URI.create(LAUNCHER_SERVER_URI + LAUNCHER_SERVER_REFRESH_TOKEN_URL);
        return sendRequest(url, requestDto, HttpMethod.POST, AuthenticationResponseDto.class);
    }

    @Override
    public UserInfoDto sendUpdateUserInfoRequest() {
        URI url = URI.create(LAUNCHER_SERVER_URI + LAUNCHER_SERVER_UPDATE_USER_INFO_URL);
        return sendRequest(url, HttpMethod.GET, UserInfoDto.class);
    }

    @Override
    public ServerImagesResponseDto sendGetPossiblePrizesRequest() {
        URI url = URI.create(LAUNCHER_SERVER_URI + LAUNCHER_SERVER_GET_POSSIBLE_PRIZES_URL);
        return sendRequest(url, HttpMethod.GET, ServerImagesResponseDto.class);
    }

    @Override
    public ServerImagesResponseDto sendGetDonateServicesRequest() {
        URI url = URI.create(LAUNCHER_SERVER_URI + LAUNCHER_SERVER_GET_DONATE_SERVICES_URL);
        return sendRequest(url, HttpMethod.GET, ServerImagesResponseDto.class);
    }

    @Override
    public LoaderSliderInfoResponseDto sendGetLoaderSliderInfoRequest() {
        URI url = URI.create(LAUNCHER_SERVER_URI + LAUNCHER_SERVER_GET_LOADER_SLIDER_INFO_SERVICES_URL);
        return sendRequest(url, HttpMethod.GET, LoaderSliderInfoResponseDto.class);
    }

    @Override
    public SpinRouletteResponseDto sendSpinRouletteRequest() {
        URI url = URI.create(LAUNCHER_SERVER_URI + LAUNCHER_SERVER_SPIN_ROULETTE_URL);
        return sendRequest(url, HttpMethod.POST, SpinRouletteResponseDto.class);
    }

    private <Response, Request> Response sendRequest(URI url, Request body, HttpMethod httpMethod, Class<Response> responseEntityClass) {
        HttpEntity<?> httpEntity = new HttpEntity<>(body, buildHeaders());
        ResponseEntity<Response> response = restTemplate.exchange(url, httpMethod, httpEntity, responseEntityClass);
        return response.getBody();
    }

    private <Response> Response sendRequest(URI url, HttpMethod httpMethod, Class<Response> responseEntityClass) {
        HttpEntity<?> httpEntity = new HttpEntity<>(buildHeaders());
        ResponseEntity<Response> response = restTemplate.exchange(url, httpMethod, httpEntity, responseEntityClass);
        return response.getBody();
    }

    private HttpHeaders buildHeaders() {
        HttpHeaders httpHeaders = new HttpHeaders();
        httpHeaders.add(AUTHORIZATION_HEADER, Storage.getProperty(StorageElements.ACCESS_TOKEN.getValue(), activity));
        return httpHeaders;
    }
}
