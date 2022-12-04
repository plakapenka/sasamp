package com.liverussia.launcher.domain.enums;

import lombok.Getter;
import lombok.RequiredArgsConstructor;

@Getter
@RequiredArgsConstructor
public enum StorageElements {
    NICKNAME("nickname"),
    EMAIL("email"),

    AUTHENTICATED_NICKNAME("authenticated_nickname"),
    AUTHENTICATED_SERVER("authenticated_server"),
    USER_BALANCE("user_balance"),

    ACCESS_TOKEN("access_token"),
    REFRESH_TOKEN("refresh_token"),

    TOKENS_RECEIVE_TIME("token_receive_time");

    private final String value;
}
