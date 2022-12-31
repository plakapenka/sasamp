package com.liverussia.launcher.domain.enums;

import lombok.Getter;
import lombok.RequiredArgsConstructor;

@Getter
@RequiredArgsConstructor
public enum  NativeStorageElements {
    NICKNAME("name"),
    SERVER("server"),
    TEST("test");

    private final String value;
}
