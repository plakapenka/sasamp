package com.liverussia.launcher.dto.response;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

import lombok.Data;

@Data
@JsonIgnoreProperties(ignoreUnknown = true)
public class AuthenticationResponseDto {

    private String accessToken;

    private String refreshToken;

    private UserInfoDto userInfo;

}
