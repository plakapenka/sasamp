package com.liverussia.launcher.activity;

import android.Manifest;
import android.app.DownloadManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.database.ContentObserver;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.liverussia.cr.R;


import com.liverussia.cr.core.Utils;

import static com.liverussia.cr.core.Config.PATH_DOWNLOADS;
import static com.liverussia.cr.core.Config.URL_FILES;

import java.io.FileInputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class LoaderActivity extends AppCompatActivity {
    private static final int DATA_STATE_NONE = 0;
    private static final int DATA_STATE_DOWNLOADING = 1;
    private static final int DATA_STATE_DOWNLOAD_SUCESS = 2;

    private DownloadManager mgr = null;
    private long downloadid = 0;
    private TextView loading;
    private TextView loadingPercent;
    private ProgressBar progressBar;
    private TextView fileName;
    private ImageView leftButton;
    private ImageView rightButton;

    Uri resource = Uri.parse(URL_FILES);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_loader);
        progressBar = findViewById(R.id.progressBar);
        loadingPercent = findViewById(R.id.loadingPercent);

        registerReceiver(onComplete,
                new IntentFilter(DownloadManager.ACTION_DOWNLOAD_COMPLETE));

        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);

        //initialize(savedInstanceState);
        if (Build.VERSION.SDK_INT >= 23) {
            if (checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED
                    || checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED) {
                requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1000);
            } else {
                InstallGame();
            }
        } else {
            InstallGame();
        }
    }

    BroadcastReceiver onComplete=new BroadcastReceiver() {
        public void onReceive(Context ctxt, Intent intent) {
            Utils.setType(DATA_STATE_DOWNLOAD_SUCESS);
            InstallGame();
        }
    };
    private void StartGame() {
        Intent intent = new Intent(this, com.liverussia.cr.core.GTASA.class);
        intent.putExtras(getIntent());
        startActivity(intent);
        finish();
    }

    @Override
    public void onStop() {
        //  mediaPlayer.stop();
        super.onStop();
    }


    private void InstallGame() {
        int type = Utils.getType();
        switch (type) {
            case DATA_STATE_NONE: {
                DownloadManager mDownloadManager = (DownloadManager) getSystemService(DOWNLOAD_SERVICE);

                DownloadManager.Request request = new DownloadManager.Request(resource);

                request.setDestinationInExternalPublicDir(PATH_DOWNLOADS, "tempcache.png");

                request.setAllowedNetworkTypes(DownloadManager.Request.NETWORK_MOBILE | DownloadManager.Request.NETWORK_WIFI);
                request.setNotificationVisibility(DownloadManager.Request.VISIBILITY_VISIBLE_NOTIFY_COMPLETED);
                request.setVisibleInDownloadsUi(true);

                downloadid = mDownloadManager.enqueue(request);

                progressBar.setVisibility(View.VISIBLE);
                progressBar.setIndeterminate(false);
                progressBar.setMax(100);

                Uri myDownloads = Uri.parse("content://downloads/my_downloads");
                getContentResolver().registerContentObserver(myDownloads, true, new DownloadObserver(new Handler(Looper.getMainLooper()), this, downloadid));
                Utils.setType(DATA_STATE_DOWNLOADING);
                break;
            }

            default:
                throw new IllegalStateException("Unexpected value: " + type);
        }
    }

    public class DownloadObserver extends ContentObserver {
        private long downid;
        private Handler handler;
        private Context context;

        public DownloadObserver(Handler handler, Context context, long downid) {
            super(handler);
            this.handler = handler;
            this.downid = downid;
            this.context = context;
        }


        @Override
        public void onChange(boolean selfChange) {
            DownloadManager.Query query = new DownloadManager.Query().setFilterById(downloadid);
            DownloadManager downloadManager = (DownloadManager) getSystemService(Context.DOWNLOAD_SERVICE);
            //这个就是数据库查询啦
            Cursor cursor = downloadManager.query(query);
            while (cursor.moveToNext()) {
                float mDownload_so_far = cursor.getInt(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_BYTES_DOWNLOADED_SO_FAR));
                float mDownload_all = cursor.getInt(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_TOTAL_SIZE_BYTES));
                float mProgress = ( (mDownload_so_far / mDownload_all)*100 );

                loadingPercent.setText(mProgress + "%");
                progressBar.setProgress((int) mProgress);
                Log.d("avc", "mDownload_so_far = " + mDownload_so_far +", mDownload_all = " + mDownload_all + " mProgress = " + mProgress);

            }
            cursor.close();
        }
    }
}
