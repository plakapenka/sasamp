package com.sasamp.launcher.async.service;

import com.sasamp.launcher.async.dto.response.LatestVersionInfoDto;
import com.sasamp.launcher.async.dto.response.LoginGameResponse;
import com.sasamp.startMenu.servers_list.ServerData;

import java.util.List;

import retrofit2.Call;
import retrofit2.http.Field;
import retrofit2.http.FormUrlEncoded;
import retrofit2.http.GET;
import retrofit2.http.POST;

public interface NetworkService {

    @GET("https://files.liverussia.online/game_api/data.php")
    Call<List<ServerData>> getServersList();

    @GET("https://files.liverussia.online/apk/test/last_apk_info.json")
    Call<LatestVersionInfoDto> getLastApkInfo();

    @POST("/game_api/auth.php")
    @FormUrlEncoded
    Call<LoginGameResponse> gameLogin(@Field("email") String email, @Field("password") String password);

}
