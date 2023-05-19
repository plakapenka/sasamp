package com.ssmp.launcher.async.service;

import com.ssmp.startMenu.chooseChar.CharData;
import com.ssmp.startMenu.servers_list.ServerData;
import com.ssmp.launcher.async.dto.response.LatestVersionInfoDto;
import com.ssmp.launcher.async.dto.response.LoginGameResponse;

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

    @POST("/game_api/register_char.php")
    @FormUrlEncoded
    Call<String> registerChar(
            @Field("serverId") int servid,
            @Field("user_login") String login,
            @Field("user_pass") String pass,
            @Field("sex") int sex,
            @Field("firstName") String firstName,
            @Field("lastName") String lastName
    );

    @POST("/game_api/transfer_char.php")
    @FormUrlEncoded
    Call<String> transferChar(
            @Field("serverId") int servid,
            @Field("user_login") String login,
            @Field("user_pass") String password,
            @Field("nick") String nick,
            @Field("pass") String pass
    );

    @POST("/game_api/register_acc.php")
    @FormUrlEncoded
    Call<String> registerAccount(
            @Field("email") String email,
            @Field("password") String password
    );

    @POST("/game_api/auth.php")
    @FormUrlEncoded
    Call<LoginGameResponse> gameLogin(@Field("email") String email, @Field("password") String password);

    @POST("/game_api/get_chars_list.php")
    @FormUrlEncoded
    Call<List<CharData>> getChars(@Field("login") String email, @Field("pass") String password, @Field("servId") int servId);
}
