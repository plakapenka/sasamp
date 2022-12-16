package com.liverussia.launcher.async.service;

import com.liverussia.launcher.async.dto.request.LoginRequestDto;
import com.liverussia.launcher.async.dto.request.RefreshTokenRequestDto;
import com.liverussia.launcher.async.dto.response.AuthenticationResponseDto;
import com.liverussia.launcher.async.dto.response.LatestVersionInfoDto;
import com.liverussia.launcher.async.dto.response.LoaderSliderInfoResponseDto;
import com.liverussia.launcher.async.dto.response.MonitoringData;
import com.liverussia.launcher.async.dto.response.ServerImagesResponseDto;
import com.liverussia.launcher.async.dto.response.SpinRouletteResponseDto;
import com.liverussia.launcher.async.dto.response.UserInfoDto;
import com.liverussia.launcher.async.dto.response.News;
import com.liverussia.launcher.async.dto.response.Servers;

import java.util.List;

import retrofit2.Call;
import retrofit2.http.Body;
import retrofit2.http.GET;
import retrofit2.http.Header;
import retrofit2.http.POST;

public interface NetworkService {

    @GET("https://files.liverussia.online/launcher/data.php")
    Call<MonitoringData> getMonitoringData();

    //  @GET("https://files.liverussia.online/servers1.php")
    //Call<List<Servers>> getServers();

    @GET("https://files.liverussia.online/historyforlauncher/history.json")
    Call<List<News>> getNews();

    @GET("https://files.liverussia.online/loader_slider/texts.json")
    Call<LoaderSliderInfoResponseDto> getLoaderSliderInfo();

    @GET("https://files.liverussia.online/donate/services/urls.json")
    Call<ServerImagesResponseDto> getDonateServices();

    @GET("https://files.liverussia.online/roulette/prizes/urls.json")
    Call<ServerImagesResponseDto> getPossibleRoulettePrizes();

    @GET("https://files.liverussia.online/apk/last_apk_info.json")
    Call<LatestVersionInfoDto> getLatestVersionInfoDto();

    @POST("/api/v1/auth/android/refresh")
    Call<AuthenticationResponseDto> refreshTokens(@Body RefreshTokenRequestDto request);

    @POST("/api/v1/auth/android/login")
    Call<AuthenticationResponseDto> loginUser(@Body LoginRequestDto loginRequestDto);

    @GET("/api/v1/android/user/info")
    Call<UserInfoDto> updateUserInfo(@Header("Authorization") String token);

    @POST("/api/v1/android/user/roulette/spin")
    Call<SpinRouletteResponseDto> spinRoulette(@Header("Authorization") String token);
}
