package com.liverussia.cr.core;

import android.os.Bundle;
import android.util.Log;

public class Samp extends GTASA
{
    native void initSAMP(String game_path);

    @Override
    public void onCreate(Bundle bundle) {

        Log.i("java", "calling initSAMP");
        initSAMP(getExternalFilesDir(null).toString()+"/");

        super.onCreate(bundle);
    }
}
