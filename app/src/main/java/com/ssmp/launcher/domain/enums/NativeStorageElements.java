package com.ssmp.launcher.domain.enums;

import lombok.Getter;
import lombok.RequiredArgsConstructor;

@Getter
@RequiredArgsConstructor
public enum  NativeStorageElements {
    NICKNAME("name"),
    SERVER("server"),
    LOCKED_SERVER_PASSWORD("password"),
    TEST("test");

    private final String value;
}
