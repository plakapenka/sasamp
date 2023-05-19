package com.sasamp.launcher.async.dto.request;

import lombok.Data;

@Data
public class LoginRequestDto {

    private String login;

    private String password;

    private String serverId;

    private String captchaToken;

}
