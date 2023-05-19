package com.ssmp.launcher.utils;

import static com.ssmp.launcher.config.Config.LIVE_RUSSIA_RESOURCE_SERVER_URL;

import com.ssmp.cr.core.GTASA;
import com.ssmp.launcher.async.dto.response.FileInfo;
import com.ssmp.launcher.async.dto.response.LatestVersionInfoDto;
import com.ssmp.launcher.domain.enums.DownloadType;

import java.util.List;

import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;


public class MainUtils {
    public static GTASA currentContext;
    public static DownloadType TYPE = DownloadType.LOAD_ALL_CACHE;
    public static List<FileInfo> FILES_TO_RELOAD;
    public static LatestVersionInfoDto LATEST_APK_INFO;

    public static Retrofit retrofit = new Retrofit.Builder()
            .baseUrl(LIVE_RUSSIA_RESOURCE_SERVER_URL)
            .addConverterFactory(GsonConverterFactory.create())
            .build();

    public static DownloadType getType() {
        return TYPE;
    }

    public static void setType(DownloadType value) {
        TYPE = value;
    }
}
