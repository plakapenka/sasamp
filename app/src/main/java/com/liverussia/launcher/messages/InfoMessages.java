package com.liverussia.launcher.messages;

import lombok.Getter;
import lombok.RequiredArgsConstructor;

@Getter
@RequiredArgsConstructor
public enum  InfoMessages {
    CONNECTION_TO_LAST_SERVER("Подключение к последнему выбранному серверу!"),
    CONNECTION_TO_SELECT_SERVER("Подключение к серверу: %s!"),
    GAME_FILES_DOWNLOAD_SUCCESS("Файлы загружены. Запуск игры..."),
    INSTALL_GAME_FIRST("Сначала установите игру"),
    SETTINGS_ALREADY_DEFAULT("Настройки по умолчанию уже установлены"),
    SUCCESSFULLY_SETTINGS_RESET("Вы успешно сбросили настройки!"),
    APPROVE_INSTALL("Подтвердите установку");

    private final String text;

    public static String createConnectToSelectServerMessage(String serverName) {
        return String.format(
                CONNECTION_TO_SELECT_SERVER.getText(),
                serverName
        );
    }
}
