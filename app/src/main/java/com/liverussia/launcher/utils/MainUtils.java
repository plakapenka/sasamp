package com.liverussia.launcher.utils;

import com.liverussia.cr.core.GTASA;
import com.liverussia.launcher.async.dto.response.FileInfo;
import com.liverussia.launcher.async.dto.response.LatestVersionInfoDto;
import com.liverussia.launcher.async.dto.response.News;
import com.liverussia.launcher.async.dto.response.Servers;
import com.liverussia.launcher.domain.enums.DownloadType;

import java.util.ArrayList;
import java.util.List;


public class MainUtils {
    public static GTASA currentContext;
    public static DownloadType TYPE = DownloadType.LOAD_ALL_CACHE;
    public static List<FileInfo> FILES_TO_RELOAD;
    public static LatestVersionInfoDto LATEST_APK_INFO;
    public static List<News> NEWS = new ArrayList<>();
    public static List<Servers> SERVERS = new ArrayList<>();

    public static DownloadType getType() {
        return TYPE;
    }

    public static void setType(DownloadType value) {
        TYPE = value;
    }
}
