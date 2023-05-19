package com.sasamp.launcher.error.apiException;

import org.springframework.http.HttpStatus;

import java.util.Arrays;

import lombok.AllArgsConstructor;
import lombok.Getter;

@Getter
@AllArgsConstructor
public enum ErrorContainer {

    AUTHENTICATION_ERROR(210, "Ошибка авторизации", HttpStatus.UNAUTHORIZED),
    BAD_REQUEST(211, "Неккоретный запрос", HttpStatus.BAD_REQUEST),
    USER_NOT_FOUND(212, "Имя пользователя или пароль не верный", HttpStatus.NOT_FOUND),
    USER_WITH_THIS_LOGIN_EXIST(213, "Пользователь с таким логином уже зарегистрирован", HttpStatus.BAD_REQUEST),
    PAGE_NOT_FOUND(214, "Старница не найдена", HttpStatus.NOT_FOUND),
    ACCESS_DENIED(215, "Доступ запрещен", HttpStatus.FORBIDDEN),
    USER_WITH_THIS_LOGIN_ALREADY_REGISTER(216, "Пользователь уже зарегистрирован", HttpStatus.BAD_REQUEST),
    USER_WITH_THIS_LOGIN_NOT_FOUND(217, "Пользователь с таким логином не найден", HttpStatus.BAD_REQUEST),
    TOKEN_EXPIRED(218, "Срок действия токена истек", HttpStatus.UNAUTHORIZED),
    CAPTCHA_ERROR(219, "Попробуйте заново пройти google captcha", HttpStatus.UNAUTHORIZED),
    NOT_ENOUGH_MONEY(220, "Недостаточно денег", HttpStatus.BAD_REQUEST),
    NETWORK_ERROR(221, "Не удалось загрузить данные. Попробуйте еще раз", HttpStatus.NOT_FOUND),
    USER_IN_GAME_ERROR(222, "Выйдите из игры со всех устройств или дождитесь отключения от сервера", HttpStatus.BAD_REQUEST),

    //TODO мб вынести это в отдельный enum (подумать как на досуге) либо ошибки не содержащие код писать под кодом > 1000
    SERVER_CONNECT_ERROR(1000, "Отсуствует интернет соединение. Для решения: попробуйте переподключить роутер, либо сменить интернет, либо воспользоваться VPN", HttpStatus.NOT_FOUND),
    DOWNLOAD_FILES_ERROR(1001, "Произошла ошибка при скачивании файлов", HttpStatus.NOT_FOUND),
    DOWNLOAD_WAS_INTERRUPTED(1002, "Загрузка была прервана", HttpStatus.NOT_FOUND),
    FILE_NOT_FOUND(1003, "Файл не найден", HttpStatus.NOT_FOUND),
    INTERNET_WAS_DISABLE(1004, "Возникли проблемы с подключением, попробуйте еще раз", HttpStatus.FORBIDDEN),
    NO_SPACE_LEFT_ON_DEVICE(1005, "Недостаточно памяти на телефоне", HttpStatus.NOT_FOUND),

    OTHER(999, "Неизвестная ошибка", HttpStatus.INTERNAL_SERVER_ERROR);

    private static final ErrorContainer[] VALUES = values();

    private final int code;

    private final String message;

    private final HttpStatus httpStatus;

    
    public static ErrorContainer of(int errorCode) {
        return Arrays.stream(VALUES)
                .filter(errorContainer ->
                        errorContainer.getCode() == errorCode)
                .findFirst()
                .orElse(null);
    }

    @Override
    public String toString() {
        return "ErrorContainer{" +
                "message='" + message + '\'' +
                ", code=" + code +
                '}';
    }
}
