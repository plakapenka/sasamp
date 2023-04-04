package com.liverussia.launcher.storage;

import static com.liverussia.cr.core.Samp.activity;

import android.content.Context;
import android.content.SharedPreferences;

import com.liverussia.launcher.domain.enums.StorageElements;

import org.apache.commons.lang3.StringUtils;

public class Storage {
    public static final String STORAGE_NAME = "LiveRussia";

    public static void addProperty(StorageElements element, String value, Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        editor.putString(element.getValue(), value);
        editor.apply();
    }

    public static void remove(String value) {
        SharedPreferences sharedPreferences = activity.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        editor.remove(value);
        editor.apply();
    }

    public static void addProperty(StorageElements element, Boolean value, Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        editor.putBoolean(element.getValue(), value);
        editor.apply();
    }

    public static String getString(StorageElements element, Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getString(element.getValue(), null);
    }
    ////////////////
    public static String getString(String element) {
        SharedPreferences sharedPreferences = activity.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getString(element, null);
    }
    public static boolean getBool(String element) {
        SharedPreferences sharedPreferences = activity.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(element, false);
    }
    public static void setInt(String element, int value) {
        SharedPreferences sharedPreferences = activity.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        editor.putInt(element, value);
        editor.apply();
    }
    public static void setString(String element, String value) {
        SharedPreferences sharedPreferences = activity.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        editor.putString(element, value);
        editor.apply();
    }
    public static void setBool(String element, Boolean value) {
        SharedPreferences sharedPreferences = activity.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        editor.putBoolean(element, value);
        editor.apply();
    }

    public static int getInt(String element) {
        SharedPreferences sharedPreferences = activity.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getInt(element, 0);
    }
    public static void setProperty(Context context, String element, String value) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        editor.putString(element, value);
        editor.apply();
    }
    public static void setProperty(Context context, String element, int value) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        editor.putInt(element, value);
        editor.apply();
    }

    public static Boolean getBooleanProperty(StorageElements element, Context context) {
        SharedPreferences sharedPreferences = context.getSharedPreferences(STORAGE_NAME, Context.MODE_PRIVATE);
        return sharedPreferences.getBoolean(element.getValue(), true);
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
