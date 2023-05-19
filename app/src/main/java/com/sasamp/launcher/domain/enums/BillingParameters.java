package com.sasamp.launcher.domain.enums;

import lombok.Getter;
import lombok.RequiredArgsConstructor;

@Getter
@RequiredArgsConstructor
public enum BillingParameters {
    NICKNAME("nickname"),
    EMAIL("email"),
    DONATE_SUM("donateSum"),
    SERVER_SELECTED_ID("serverSelectedId"),
    SERVER_SELECTED_NAME("serverSelectedName"),
    CAPTCHA("captcha");

    private final String name;

}
