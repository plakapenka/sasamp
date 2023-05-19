package com.ssmp.launcher.domain.inscriptions;

import lombok.Getter;
import lombok.RequiredArgsConstructor;

@Getter
@RequiredArgsConstructor
public enum Inscriptions {
    BALANCE("Баланс: %s LC");

    private final String text;

    public static String createBalanceInscription(String balance) {
        return String.format(
                BALANCE.getText(),
                balance
        );
    }
}
