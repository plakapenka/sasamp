package com.liverussia.launcher.async.dto.response;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

import lombok.Data;

@Data
@JsonIgnoreProperties(ignoreUnknown = true)
public class UserInfoDto {

    private String username;

    private String serverName;

    private String balance;
}
