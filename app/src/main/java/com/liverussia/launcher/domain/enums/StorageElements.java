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
    SERVER_MULTI("server_multi"),
    SERVER_NAME("server_name"),
    SERVER_COLOR("server_color"),
    SERVER_LOCKED("server_lock"),

    ACCESS_TOKEN("access_token"),
    REFRESH_TOKEN("refresh_token"),

    ROULETTE_SERVER_HOST("roulette_server"),

    TOKENS_RECEIVE_TIME("token_receive_time"),
    IS_CHECK_FILES_ON("is_check_files_on");

    private final String value;
}
