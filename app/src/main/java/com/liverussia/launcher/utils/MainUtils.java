package com.liverussia.launcher.utils;

import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;

import com.liverussia.launcher.async.dto.response.FileInfo;
import com.liverussia.launcher.async.dto.response.LatestVersionInfoDto;
import com.liverussia.launcher.async.dto.response.News;
import com.liverussia.launcher.async.dto.response.Servers;
import com.liverussia.launcher.domain.enums.DownloadType;

import java.util.ArrayList;
import java.util.List;


public class MainUtils extends AppCompatActivity {
    public static AppCompatActivity currentContext;
    public static DownloadType TYPE = DownloadType.LOAD_ALL_CACHE;
    public static List<FileInfo> FILES_TO_RELOAD;
    public static LatestVersionInfoDto LATEST_APK_INFO;
    public static List<News> NEWS = new ArrayList<>();
    public static List<Servers> SERVERS = new ArrayList<>();

    protected void onCreate(Bundle bundle) {
        super.onCreate(bundle);
    }

    public static DownloadType getType() {
        return TYPE;
    }

    public static void setType(DownloadType value) {
        TYPE = value;
    }
}
