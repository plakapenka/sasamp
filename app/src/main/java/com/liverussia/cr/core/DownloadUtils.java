package com.liverussia.cr.core;

import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;

import com.liverussia.launcher.dto.response.FileInfo;
import com.liverussia.launcher.dto.response.LatestVersionInfoDto;
import com.liverussia.launcher.enums.DownloadType;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import static com.liverussia.cr.core.Config.GAME_PATH;


public class DownloadUtils extends AppCompatActivity {
    public static AppCompatActivity currentContext;
    public static DownloadType TYPE = DownloadType.LOAD_ALL_CACHE;
    public static List<FileInfo> FILES_TO_RELOAD;
    public static LatestVersionInfoDto LATEST_APK_INFO;

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
