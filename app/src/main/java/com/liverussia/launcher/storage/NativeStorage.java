package com.liverussia.launcher.storage;

import android.app.Activity;
import android.widget.Toast;

import com.liverussia.launcher.enums.NativeStorageElements;
import com.liverussia.launcher.error.apiException.ApiException;
import com.liverussia.launcher.error.apiException.ErrorContainer;
import com.liverussia.launcher.messages.ErrorMessages;

import org.ini4j.Wini;

import java.io.File;
import java.io.IOException;

import static com.liverussia.launcher.config.Config.NATIVE_SETTINGS_FILE_PATH;

public class NativeStorage {

    private static final String CLIENT_SECTION_NAME = "client";



    public static void addClientProperty(NativeStorageElements propertyName, String value, Activity activity) {
        try {
            File f = new File(activity.getExternalFilesDir(null) + NATIVE_SETTINGS_FILE_PATH);

            if (!f.exists()) {
                throw new ApiException(ErrorContainer.FILE_NOT_FOUND);
            }

            Wini w = new Wini(new File(activity.getExternalFilesDir(null) + NATIVE_SETTINGS_FILE_PATH));
            w.put(CLIENT_SECTION_NAME, propertyName.getValue(), value);
            w.store();
        } catch (ApiException e) {
            showMessage(ErrorMessages.FIRSTLY_LOAD_GAME.getText(), activity);
        } catch (IOException e) {
            showMessage(ErrorMessages.FIRSTLY_LOAD_GAME.getText(), activity);
        }
    }

    public static String getClientProperty(NativeStorageElements property, Activity activity) {

        String value = null;

        try {
            Wini w = new Wini(new File(activity.getExternalFilesDir(null) + NATIVE_SETTINGS_FILE_PATH));
            value = w.get(CLIENT_SECTION_NAME, property.getValue());
            w.store();
        } catch (IOException ignored) {

        }

        return value;
    }

    private static void showMessage(String message, Activity activity) {
        Toast.makeText(activity, message, Toast.LENGTH_SHORT)
                .show();
    }
}
