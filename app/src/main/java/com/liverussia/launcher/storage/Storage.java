package com.liverussia.launcher.storage;

import android.content.Context;
import android.content.SharedPreferences;

import org.apache.commons.lang3.StringUtils;

public class Storage {
    public static final String STORAGE_NAME = "LiveRussia";

    public static void addProperty(String name, String value, Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        editor.putString(name, value);
        editor.apply();
    }

    public static String getProperty(String name, Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getString(name, null);
    }

    public static void deleteProperty(String name, Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        for (String key : sharedPreferences.getAll().keySet()) {
            if (key.startsWith(name)) {
                editor.remove(key);
            }
        }

        editor.apply();
    }

    public static boolean isPropertyExist(String name, Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        String property = sharedPreferences.getString(name, null);

        return StringUtils.isNotBlank(property);
    }
}
