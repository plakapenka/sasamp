package com.liverussia.launcher.dto.request;

import lombok.Data;

@Data
public class LoginRequestDto {

    private String login;

    private String password;

    private String captchaToken;

}
