package com.sasamp.launcher.utils;

import static com.sasamp.launcher.config.Config.LIVE_RUSSIA_RESOURCE_SERVER_URL;

import com.sasamp.cr.core.GTASA;
import com.sasamp.launcher.async.dto.response.FileInfo;
import com.sasamp.launcher.async.dto.response.LatestVersionInfoDto;
import com.sasamp.launcher.domain.enums.DownloadType;

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
