package com.liverussia.launcher.domain;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;

import lombok.Data;

@Data
@JsonIgnoreProperties(ignoreUnknown = true)
public class Token {
    @JsonProperty(value = "sub")
    private String login;

    @JsonProperty(value = "exp")
    private String expiration;

    @JsonProperty(value = "iat")
    private String issuedAt;
}
