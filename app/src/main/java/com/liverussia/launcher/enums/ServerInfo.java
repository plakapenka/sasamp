package com.liverussia.launcher.enums;

import android.os.Build;

import androidx.annotation.RequiresApi;

import com.liverussia.cr.R;

import java.util.Arrays;

import lombok.Getter;
import lombok.RequiredArgsConstructor;

@Getter
@RequiredArgsConstructor
public enum ServerInfo {
    MOSCOW("1", "Moscow", R.drawable.moscow_logo_text),
    PETERSBURG("2", "Petersburg", R.drawable.peterburg_logo_text),
    NOVOSIBIRSK("3", "Novosibirsk", R.drawable.novosibirsk_logo_text),
    SAMARA("4", "Samara", R.drawable.samara_logo_text),
    SOCHI("5", "Sochi", R.drawable.sochi_logo_text),
    TEST("6", "Test Server", R.drawable.testserver_logo_text);

    private final String index;
    private final String name;
    private final int resource;

    public static ServerInfo ofIndex(String index) {
        return Arrays.stream(values())
                .filter(serverInfo ->
                        serverInfo.getIndex().equals(index))
                .findFirst()
                .orElse(null);
    }

    public static ServerInfo ofName(String name) {
        return Arrays.stream(values())
                .filter(serverInfo ->
                        serverInfo.getName().equals(name))
                .findFirst()
                .orElse(null);
    }

}

