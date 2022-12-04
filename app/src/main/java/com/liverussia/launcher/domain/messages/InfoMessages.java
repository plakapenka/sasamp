package com.liverussia.launcher.domain.messages;

import lombok.Getter;
import lombok.RequiredArgsConstructor;

@Getter
@RequiredArgsConstructor
public enum  InfoMessages {
    CONNECTION_TO_LAST_SERVER("Подключение к последнему выбранному серверу!"),
    CONNECTION_TO_SELECT_SERVER("Подключение к серверу: %s!"),
    SERVER_SELECTED("Сервер выбран! Для начала игры нажмите жёлтую кнопку"),
    INSTALL_GAME_FIRST("Сначала установите игру"),
    SETTINGS_ALREADY_DEFAULT("Настройки по умолчанию уже установлены"),
    SUCCESSFULLY_SETTINGS_RESET("Вы успешно сбросили настройки!"),
    DOWNLOAD_SUCCESS_INPUT_YOUR_NICKNAME("Игра успешно установлена, введите свой никнейм"),
    APPROVE_INSTALL("Подтвердите установку");

    private final String text;

    public static String createConnectToSelectServerMessage(String serverName) {
        return String.format(
                CONNECTION_TO_SELECT_SERVER.getText(),
                serverName
        );
    }
}
