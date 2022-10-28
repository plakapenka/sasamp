package com.liverussia.launcher.other;

import com.liverussia.launcher.dto.request.LoginRequestDto;
import com.liverussia.launcher.dto.request.RefreshTokenRequestDto;
import com.liverussia.launcher.dto.response.AuthenticationResponseDto;
import com.liverussia.launcher.dto.response.LoaderSliderInfoResponseDto;
import com.liverussia.launcher.dto.response.ServerImagesResponseDto;
import com.liverussia.launcher.model.News;
import com.liverussia.launcher.model.Servers;

import java.util.List;

import retrofit2.Call;
import retrofit2.http.Body;
import retrofit2.http.GET;
import retrofit2.http.Header;
import retrofit2.http.POST;

public interface NetworkService {

    @GET("https://vl4sov.com/liverussia.online/servers.json")
    Call<List<Servers>> getServers();

    @GET("https://vl4sov.com/liverussia.online/history.json")
    Call<List<News>> getNews();

    @GET("/api/v1/android/setting/loader/slider/info")
    Call<LoaderSliderInfoResponseDto> getLoaderSliderInfo();

    @GET("/api/v1/android/setting/prizes/info")
    Call<ServerImagesResponseDto> getPossiblePrizes();

    @GET("/api/v1/android/setting/donate/services")
    Call<ServerImagesResponseDto> getDonateServices();

    @POST("/api/v1/auth/android/refresh")
    Call<AuthenticationResponseDto> refreshTokens(@Body RefreshTokenRequestDto request);

    @POST("/api/v1/auth/android/login")
    Call<AuthenticationResponseDto> loginUser(@Body LoginRequestDto loginRequestDto);
}
