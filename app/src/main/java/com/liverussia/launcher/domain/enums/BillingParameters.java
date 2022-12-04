package com.liverussia.launcher.domain.enums;

import lombok.Getter;
import lombok.RequiredArgsConstructor;

@Getter
@RequiredArgsConstructor
public enum BillingParameters {
    NICKNAME("nickname"),
    EMAIL("email"),
    DONATE_SUM("donateSum"),
    SERVER_SELECTED("serverSelected");

    private final String name;

}
