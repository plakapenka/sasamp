package com.liverussia.launcher.messages;

import lombok.Getter;
import lombok.RequiredArgsConstructor;

@Getter
@RequiredArgsConstructor
public enum ErrorMessages {
    NICKNAME_IS_EMPTY("Введите никнейм"),
    EMAIL_IS_EMPTY("Введите E-mail"),
    DONATE_SUM_IS_EMPTY("Введите сумму доната"),
    SERVER_NOT_SELECTED("Выберите сервер"),
    NICKNAME_CONTAINS_DIGITS("Никнейм не должен содержать цифр"),
    DONATE_SUM_CONTAINS_SYMBOLS_OTHER_DIGITS("Сумма доната не должна содержать ничего кроме цифр"),
    NICKNAME_IS_TOO_LONG("Длина никнейма должна быть меньше 20 символов"),
    EMAIL_IS_TOO_LONG("Длина E-mail должна быть меньше 100 символов"),
    NICKNAME_DOES_NOT_MATCH_PATTERN("Никнейм не соответствует формату: \"Имя_Фамилия\""),
    EMAIL_DOES_NOT_MATCH_PATTERN("Никнейм не соответствует формату: \"example@example.com\""),
    PASSWORD_IS_EMPTY("Введите пароль"),
    PASSWORD_IS_TOO_LONG("Длина пароля должна быть меньше 17 символов"),
    PASSWORD_IS_TOO_SHORT("Длина пароля должна быть больше 5 символов"),

    INPUT_NICKNAME_BEFORE_SERVER_CONNECT("Перед подключением к серверу введите никнейм"),
    CAPTCHA_NOT_PASSED("Перед входом подтвердите что вы не робот"),
    CAPTCHA_COMPLETING_PROBLEMS("Не смогли проверить что вы не робот, попробуйте еще раз"),

    FAILED_LOAD_LOADER_SLIDER_DATA("Не удалось загрузить данные");


    private final String text;

}
