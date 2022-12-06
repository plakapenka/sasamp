package com.liverussia.launcher.storage;

import android.app.Activity;
import android.content.Context;
import android.widget.Toast;

import com.liverussia.launcher.domain.enums.NativeStorageElements;
import com.liverussia.launcher.error.apiException.ApiException;
import com.liverussia.launcher.error.apiException.ErrorContainer;
import com.liverussia.launcher.domain.messages.ErrorMessage;

import org.ini4j.Wini;

import java.io.File;
import java.io.IOException;

import static com.liverussia.launcher.config.Config.NATIVE_SETTINGS_FILE_PATH;

public class NativeStorage {

    private static final String CLIENT_SECTION_NAME = "client";



    public static void addClientProperty(NativeStorageElements propertyName, String value, Context context) {
        try {
            File f = new File(context.getExternalFilesDir(null) + NATIVE_SETTINGS_FILE_PATH);

            if (!f.exists()) {
                throw new ApiException(ErrorContainer.FILE_NOT_FOUND);
            }

            Wini w = new Wini(new File(context.getExternalFilesDir(null) + NATIVE_SETTINGS_FILE_PATH));
            w.put(CLIENT_SECTION_NAME, propertyName.getValue(), value);
            w.store();
        } catch (ApiException e) {
            showMessage(ErrorMessage.FIRSTLY_LOAD_GAME.getText(), context);
        } catch (IOException e) {
            showMessage(ErrorMessage.FIRSTLY_LOAD_GAME.getText(), context);
        }
    }

    public static String getClientProperty(NativeStorageElements property, Context context) {

        String value = null;

        try {
            Wini w = new Wini(new File(context.getExternalFilesDir(null) + NATIVE_SETTINGS_FILE_PATH));
            value = w.get(CLIENT_SECTION_NAME, property.getValue());
            w.store();
        } catch (IOException ignored) {

        }

        return value;
    }

    private static void showMessage(String message, Context context) {
        Toast.makeText(context, message, Toast.LENGTH_SHORT)
                .show();
    }
}
