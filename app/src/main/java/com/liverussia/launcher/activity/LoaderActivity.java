package com.liverussia.launcher.activity;

import static com.liverussia.cr.core.Config.URL_FILES;

import android.Manifest;
import android.annotation.SuppressLint;
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
import android.os.Environment;
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

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.nio.charset.Charset;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class LoaderActivity extends AppCompatActivity {
    private static final int DATA_STATE_NONE = 0;
    private static final int DATA_STATE_DOWNLOADING = 1;
    private static final int DATA_STATE_DOWNLOAD_SUCESS = 2;

    private DownloadManager mDownloadManager = null;
    private long downloadid = 0;
    private TextView loadingText;
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
        mDownloadManager = (DownloadManager) getSystemService(DOWNLOAD_SERVICE);
        loadingText = findViewById(R.id.loadingText);
        fileName = findViewById(R.id.fileName);

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
                // InstallGame();
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


                DownloadManager.Request request = new DownloadManager.Request(resource);

                request.setDestinationInExternalFilesDir(this, Environment.DIRECTORY_DOWNLOADS, "cache.zip");

                request.setAllowedNetworkTypes(DownloadManager.Request.NETWORK_MOBILE | DownloadManager.Request.NETWORK_WIFI);
                request.setNotificationVisibility(DownloadManager.Request.VISIBILITY_VISIBLE);
                request.setTitle("Live Russia");
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
            case DATA_STATE_DOWNLOAD_SUCESS: {

                File zipFile = new File( this.getExternalFilesDir(Environment.DIRECTORY_DOWNLOADS),"/cache.zip");
                File unzipLocation =  this.getExternalFilesDir(null);

                Log.d("Unzip", "Zipfile: " + zipFile);
                Log.d("Unzip", "location: " + unzipLocation);

                Decompress d = new Decompress(zipFile, unzipLocation);
               // d.unzip();

                loadingText.setText("Распаковка файлов игры...");
                loadingPercent.setText("0%");
                progressBar.setProgress(0);
            }
        }
    }

    public void SetStatusText(String text)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                loadingText.setText(text);
            }
        });

    }
    public void SetFileNameText(String text)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                fileName.setText(text);
            }
        });

    }
    public void SetPercentText(String text)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                    loadingPercent.setText(text);
            }
        });

    }
    public void SetProgress(int progress)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                progressBar.setProgress(progress);
            }
        });
    }
    public void SetMaxProgress(int progress)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                progressBar.setMax(progress);
            }
        });

    }

    public class Decompress extends Thread {
        private File _zipFile;
        private File _location;

        public Decompress(File zipFile, File location) {

            _zipFile = zipFile;
            _location = location;

            //_dirChecker("");

            start(); // Запускаем поток
        }

        @SuppressLint("SetTextI18n")
        public void run() {

            try {

                byte[] buffer = new byte[4096];
                FileInputStream inputStream = new FileInputStream(_zipFile);

                ZipInputStream zipStream = new ZipInputStream(inputStream, Charset.forName("windows-1251"));

                long totalSize = 3113640;

                ZipEntry zEntry = null;

                long readedByte = 0;
                //zEntry = null;
                float percent = 0;
                _dirChecker("files");
                    while ((zEntry = zipStream.getNextEntry()) != null) {
                      //  dirpart
                        //Log.d("asdf", "File = " + zEntry.getName());
                        SetFileNameText(zEntry.getName());
                        if (zEntry.isDirectory()) {
                            _dirChecker(zEntry.getName());
                        } else {
                            FileOutputStream fout = new FileOutputStream(
                                    this._location + zEntry.getName());
                            BufferedOutputStream bufout = new BufferedOutputStream(fout);

                            int read = 0;

                            while ((read = zipStream.read(buffer)) != -1) {

                                readedByte += read;
                                bufout.write(buffer, 0, read);
                                percent = ((((float)readedByte/1000) / (float)totalSize) * 100);
                                if(read % 100 == 0)
                                {
                                    String _str = String.format("%.2f %%", percent);
                                    SetPercentText(_str);
                                    SetProgress((int)percent);
                                }

                               // progressBar.setProgress(readedByte);
                            }

                            zipStream.closeEntry();
                            bufout.close();
                            fout.close();
                            SetProgress(100);
                            SetPercentText("100 %");

                        }
                    }

                zipStream.close();
                SetStatusText("Распаковка завершена");
                _zipFile.delete();
            } catch (Exception e) {
                SetStatusText("Ошибка распаковки");
                e.printStackTrace();
            }

        }

        private void _dirChecker(String dir) {
            File f = new File(_location + dir);

            if(!f.isDirectory()) {
                f.mkdirs();
            }
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
            if(Utils.getType() == DATA_STATE_DOWNLOAD_SUCESS) return ;
            DownloadManager.Query query = new DownloadManager.Query().setFilterById(downloadid);
            DownloadManager downloadManager = (DownloadManager) getSystemService(Context.DOWNLOAD_SERVICE);
            //这个就是数据库查询啦
            Cursor cursor = downloadManager.query(query);
            while (cursor.moveToNext()) {
                float mDownload_so_far = cursor.getInt(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_BYTES_DOWNLOADED_SO_FAR));
                float mDownload_all = cursor.getInt(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_TOTAL_SIZE_BYTES));

                float mProgress = ( (mDownload_so_far / mDownload_all)*100 );

                progressBar.setProgress((int) mProgress);
                String _str = String.format("%.2f %%",mProgress);
                loadingPercent.setText(_str);


            }
            cursor.close();
        }
    }

}
