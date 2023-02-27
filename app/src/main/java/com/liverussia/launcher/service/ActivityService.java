package com.liverussia.launcher.service;

import android.app.Activity;
import android.content.Context;

public interface ActivityService {

    void showMessage(String message, Context context);

    boolean isGameFileInstall(Activity activity, String filePath);

    void showBigMessage(String message, Activity activity);
}
