package com.liverussia.launcher.dto.response;

import lombok.Data;

@Data
public class AuthenticationResponseDto {

    private String accessToken;

    private String refreshToken;

    private UserInfoDto userInfo;

}
