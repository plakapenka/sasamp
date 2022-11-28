package com.liverussia.launcher.utils;

public class InputPatterns {
    public static final String NICKNAME_PATTERN = "^[A-Z][a-z]+_[A-Z][a-z]+$";
    public static final String EMAIL_PATTERN = "^(?=.{1,64}@)[A-Za-z0-9_-]+(\\.[A-Za-z0-9_-]+)*@"
            + "[^-][A-Za-z0-9-]+(\\.[A-Za-z0-9-]+)*(\\.[A-Za-z]{2,})$";
    public static final String STRING_WITH_DIGITS_PATTERN = ".*\\d+.*";

    public static final String STRING_ONLY_DIGITS_PATTERN = "\\d+";
}
