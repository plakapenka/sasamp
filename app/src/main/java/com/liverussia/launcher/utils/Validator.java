package com.liverussia.launcher.utils;

import android.app.Activity;
import android.widget.Toast;

import com.liverussia.launcher.domain.messages.ErrorMessages;
import com.liverussia.launcher.async.dto.response.Servers;

import org.apache.commons.lang3.StringUtils;

public class Validator {

    private static final int NICKNAME_MAX_LENGTH = 20;
    private static final int EMAIL_MAX_LENGTH = 100;
    private static final int PASSWORD_MAX_LENGTH = 16;
    private static final int PASSWORD_MIN_LENGTH = 6;

    public static boolean isValidNickname(String nickname, Activity activity) {

        if (StringUtils.isBlank(nickname)) {
            showMessage(ErrorMessages.NICKNAME_IS_EMPTY.getText(), activity);
            return false;
        }

        if (nickname.matches(InputPatterns.STRING_WITH_DIGITS_PATTERN)) {
            showMessage(ErrorMessages.NICKNAME_CONTAINS_DIGITS.getText(), activity);
            return false;
        }

        if (nickname.length() > NICKNAME_MAX_LENGTH) {
            showMessage(ErrorMessages.NICKNAME_IS_TOO_LONG.getText(), activity);
            return false;
        }

        if (!nickname.matches(InputPatterns.NICKNAME_PATTERN)) {
            showMessage(ErrorMessages.NICKNAME_DOES_NOT_MATCH_PATTERN.getText(), activity);
            return false;
        }

        return true;
    }

    public static boolean isValidEmail(String email, Activity activity) {

        if (StringUtils.isBlank(email)) {
            showMessage(ErrorMessages.EMAIL_IS_EMPTY.getText(), activity);
            return false;
        }

        if (email.length() > EMAIL_MAX_LENGTH) {
            showMessage(ErrorMessages.EMAIL_IS_TOO_LONG.getText(), activity);
            return false;
        }

        if (!email.matches(InputPatterns.EMAIL_PATTERN)) {
            showMessage(ErrorMessages.EMAIL_DOES_NOT_MATCH_PATTERN.getText(), activity);
            return false;
        }

        return true;
    }

    public static boolean isValidDonateSum(String donateSum, Activity activity) {

        if (StringUtils.isBlank(donateSum)) {
            showMessage(ErrorMessages.DONATE_SUM_IS_EMPTY.getText(), activity);
            return false;
        }

        if (!donateSum.matches(InputPatterns.STRING_ONLY_DIGITS_PATTERN)) {
            showMessage(ErrorMessages.DONATE_SUM_CONTAINS_SYMBOLS_OTHER_DIGITS.getText(), activity);
            return false;
        }

        return true;
    }

    public static boolean isValidPassword(String password, Activity activity) {

        if (StringUtils.isBlank(password)) {
            showMessage(ErrorMessages.PASSWORD_IS_EMPTY.getText(), activity);
            return false;
        }

        if (password.length() > PASSWORD_MAX_LENGTH) {
            showMessage(ErrorMessages.PASSWORD_IS_TOO_LONG.getText(), activity);
            return false;
        }

        if (password.length() < PASSWORD_MIN_LENGTH) {
            showMessage(ErrorMessages.PASSWORD_IS_TOO_SHORT.getText(), activity);
            return false;
        }

        return true;
    }

    public static boolean isValidSelectedServer(Servers selectedServer, Activity activity) {

        if (selectedServer == null) {
            showMessage(ErrorMessages.SERVER_NOT_SELECTED.getText(), activity);
            return false;
        }

        return true;
    }

    public static boolean isValidCaptchaToken(String captchaToken, Activity activity) {

        if (StringUtils.isBlank(captchaToken)) {
            showMessage(ErrorMessages.CAPTCHA_NOT_PASSED.getText(), activity);
            return false;
        }

        return true;
    }

    private static void showMessage(String message, Activity activity) {
        Toast.makeText(activity, message, Toast.LENGTH_SHORT)
                .show();
    }
}
