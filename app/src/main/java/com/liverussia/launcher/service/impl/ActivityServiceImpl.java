package com.liverussia.launcher.service.impl;

import android.app.Activity;
import android.content.Context;
import android.widget.Toast;

import com.liverussia.launcher.service.ActivityService;

import java.io.File;

public class ActivityServiceImpl implements ActivityService {

    @Override
    public void showMessage(String message, Context context) {
        Toast.makeText(context, message, Toast.LENGTH_SHORT)
                .show();
    }

    @Override
    public void showBigMessage(String message, Activity activity) {
        Toast.makeText(activity, message, Toast.LENGTH_LONG)
                .show();
    }

    @Override
    public boolean isGameFileInstall(Activity activity, String filePath) {
        String fileFullPath = activity.getExternalFilesDir(null).toString()
                .concat(filePath);

        File file = new File(fileFullPath);
        return file.exists();
    }
}
