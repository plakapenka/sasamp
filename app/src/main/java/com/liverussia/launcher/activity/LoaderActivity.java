package com.liverussia.launcher.activity;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.os.PowerManager;
import android.util.Log;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.FileProvider;

import com.liverussia.cr.BuildConfig;
import com.liverussia.cr.R;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.Charset;
import java.text.DecimalFormat;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import com.liverussia.cr.core.Utils;
import com.nvidia.devtech.NvEventQueueActivity;

import static android.widget.Toast.LENGTH_LONG;
import static com.liverussia.cr.core.Config.APP_PATH;
import static com.liverussia.cr.core.Config.GAME_PATH;
import static com.liverussia.cr.core.Config.PATH_DOWNLOADS;
import static com.liverussia.cr.core.Config.URL_CLIENT;
import static com.liverussia.cr.core.Config.URL_FILES;
import static com.liverussia.cr.core.Config.URL_FILES_UPDATE;

public class LoaderActivity extends AppCompatActivity {
    private TextView loading;
    private TextView loadingPercent;
    private ProgressBar progressBar;
    private TextView fileName;
    private ImageView leftButton;
    private ImageView rightButton;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_loader);

        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);

        initialize(savedInstanceState);
        if (Build.VERSION.SDK_INT >= 23) {
            if (checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED
                    || checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED) {
                requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1000);
            } else {
                InstallGame(Utils.getType());
            }
        } else {
            InstallGame(Utils.getType());
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 1000) {
            InstallGame(Utils.getType());
        }
    }

    private static class infoclass {
        public static int fileLengthFull;
        public static int fileLengthMin;
        public static String str;
    }

    public static void writeFile(String path, String str) {
        File file = new File(path);

        try {
            if (!file.exists())
                file.createNewFile();
        } catch (IOException e) {
            e.printStackTrace();
        }
        FileWriter fileWriter = null;

        try {
            fileWriter = new FileWriter(new File(path), false);
            fileWriter.write(str);
            fileWriter.flush();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (fileWriter != null)
                    fileWriter.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public static void writeLog(Exception e){
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        e.printStackTrace(pw);
        String text = sw.toString();
        writeFile(APP_PATH+"/log.txt", text);
    }

    private void StartGame()
    {
        Intent intent = new Intent(this, com.liverussia.cr.core.GTASA.class);
        intent.putExtras(getIntent());
        startActivity(intent);
        finish();
    }

    @Override
    public void onStop ()
    {
        //  mediaPlayer.stop();
        super.onStop();
    }

    private void InstallGame(int type)
    {
        switch (type) {
            case 0:
            {
                File dir = new File(PATH_DOWNLOADS);
                if (!dir.exists()) dir.mkdirs();
                final DownloadTask downloadTask = new DownloadTask(this);
                downloadTask.execute(URL_FILES);
                break;
            }
            case 1:
            {
                // Install UPDATE APK
                File dir = new File(PATH_DOWNLOADS);
                if (!dir.exists()) dir.mkdirs();
                final DownloadTask downloadTask = new DownloadTask(this);
                downloadTask.execute(URL_CLIENT);
                break;
            }
            case 2:
            {
                File dir = new File(PATH_DOWNLOADS);
                if (!dir.exists()) dir.mkdirs();
                final DownloadTask downloadTask = new DownloadTask(this);
                downloadTask.execute(URL_FILES_UPDATE);
                break;
            }
        }
    }

    private void initialize(Bundle savedInstanceState) {
        Animation animation = AnimationUtils.loadAnimation(this, R.anim.button_click);
        loading = (TextView) findViewById(R.id.loadingText);
        loadingPercent = (TextView) findViewById(R.id.loadingPercent);
        progressBar = (ProgressBar) findViewById(R.id.progressBar);
        fileName = (TextView) findViewById(R.id.fileName);
        leftButton = (ImageView) findViewById(R.id.leftButton);
        rightButton = (ImageView) findViewById(R.id.rightButton);

        leftButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                v.startAnimation(animation);
                // function
            }
        });

        rightButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                v.startAnimation(animation);
                // function
            }
        });
    }


    public void UnZipZip()
    {
        final UnZipTask unzipTask = new UnZipTask(this);
        if(Utils.getType() == 0) unzipTask.execute(PATH_DOWNLOADS+"game.zip", GAME_PATH);
        else unzipTask.execute(PATH_DOWNLOADS+"upd.zip", GAME_PATH);
    }

    private void InstallAPK(final String apkname) {
        try {
            File file = new File(PATH_DOWNLOADS, apkname + ".apk");
            Intent intent;
            if (file.exists()) {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                    Uri apkUri = FileProvider.getUriForFile(this, BuildConfig.APPLICATION_ID + ".provider", file);
                    intent = new Intent(Intent.ACTION_INSTALL_PACKAGE);
                    intent.setData(apkUri);
                    intent.setFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
                } else {
                    Uri apkUri = Uri.fromFile(file);
                    intent = new Intent(Intent.ACTION_VIEW);
                    intent.setDataAndType(apkUri, "application/vnd.android.package-archive");
                    intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                }
                startActivity(intent);
                finish();
            }
            else showMessage("Ошибка установки: файл не найден");
        } catch (Exception e) {
            Log.e("InstallAPK", "Ошибка установки:" + e.getMessage());
            writeLog(e);
        }
    }

    public void showMessage(String _s) {
        Toast.makeText(getApplicationContext(), _s, Toast.LENGTH_LONG).show();
    }

    public static String formatFileSize(long size) {
        String hrSize = null;

        double b = size;
        double k = size/1024.0;
        double m = ((size/1024.0)/1024.0);
        double g = (((size/1024.0)/1024.0)/1024.0);
        double t = ((((size/1024.0)/1024.0)/1024.0)/1024.0);

        DecimalFormat dec = new DecimalFormat("0.00");

        if ( t>1 ) {
            hrSize = dec.format(t).concat(" TB");
        } else if ( g>1 ) {
            hrSize = dec.format(g).concat(" GB");
        } else if ( m>1 ) {
            hrSize = dec.format(m).concat(" MB");
        } else if ( k>1 ) {
            hrSize = dec.format(k).concat(" KB");
        } else {
            hrSize = dec.format(b).concat(" Bytes");
        }

        return hrSize;
    }

    private class DownloadTask extends AsyncTask<String, Integer, String> {

        private Context context;
        private PowerManager.WakeLock mWakeLock;

        public DownloadTask(Context context) {
            this.context = context;
        }

        @Override
        protected String doInBackground(String... sUrl) { // doInAnal
            InputStream input = null;
            OutputStream output = null;
            HttpURLConnection connection = null;
            try {
                URL url = new URL(sUrl[0]);
                connection = (HttpURLConnection) url.openConnection();
                connection.connect();
                if (connection.getResponseCode() != HttpURLConnection.HTTP_OK) {
                    return "Server returned HTTP " + connection.getResponseCode()
                            + " " + connection.getResponseMessage();
                }
                int fileLength = connection.getContentLength();
                infoclass.fileLengthFull = fileLength;
                input = connection.getInputStream();
                if(Utils.getType() == 0) {
                    File file = new File(PATH_DOWNLOADS, "game.zip");
                    if (file.exists()) file.delete();
                    output = new FileOutputStream(PATH_DOWNLOADS + "game.zip");
                }
                else if(Utils.getType() == 1) {
                    File file = new File(PATH_DOWNLOADS, "launcher.apk");
                    if (file.exists()) file.delete();
                    output = new FileOutputStream(PATH_DOWNLOADS + "launcher.apk");
                }
                else if(Utils.getType() == 2) {
                    File file = new File(PATH_DOWNLOADS, "upd.zip");
                    if (file.exists()) file.delete();
                    output = new FileOutputStream(PATH_DOWNLOADS + "upd.zip");
                }
                byte data[] = new byte[4096];
                long total = 0;
                int count;
                while ((count = input.read(data)) != -1) {
                    if (isCancelled()) {
                        input.close();
                        return null;
                    }
                    total += count;
                    if (fileLength > 0)
                        publishProgress((int) (total * 100 / fileLength));
                    infoclass.fileLengthMin = (int) ((total * 100 / fileLength) * (fileLength / 100));
                    output.write(data, 0, count);
                }
            } catch (Exception e) {
                return e.toString();
            } finally {
                try {
                    if (output != null)
                        output.close();
                    if (input != null)
                        input.close();
                } catch (IOException ignored) {
                }

                if (connection != null)
                    connection.disconnect();
            }
            return null;
        }

        @Override
        protected void onPreExecute()
        {
            super.onPreExecute();
            PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
            mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, getClass().getName());
            mWakeLock.acquire();
            progressBar.setVisibility(View.VISIBLE);
            progressBar.setIndeterminate(false);
            progressBar.setMax(100);
        }

        @Override
        protected void onProgressUpdate(Integer... progress)
        {
            super.onProgressUpdate(progress);
            if(Utils.getType() == 0)
            {
                loading.setText("Загрузка файлов игры...");
                loadingPercent.setText(progress[0] + "%");
                fileName.setText(formatFileSize(infoclass.fileLengthMin)+" из "+formatFileSize(infoclass.fileLengthFull));
            }
            if(Utils.getType() == 1)
            {
                loading.setText("Загрузка лаунчера...");
                loadingPercent.setText(progress[0] + "%");
            }
            if(Utils.getType() == 2)
            {
                loading.setText("Загрузка доп.файлов...");
                loadingPercent.setText(progress[0] + "%");
            }
            progressBar.setProgress(progress[0]);
        }

        @Override
        protected void onPostExecute(String result) {
            mWakeLock.release();
            loading.setVisibility(View.INVISIBLE);
            loadingPercent.setVisibility(View.INVISIBLE);
            progressBar.setVisibility(View.INVISIBLE);
            if(Utils.getType() == 0 || Utils.getType() == 2)
            {
                if (result != null) Toast.makeText(context, "Ошибка загрузки: " + result, LENGTH_LONG).show();
                else Toast.makeText(context, "Файлы загружены.Распаковка..", Toast.LENGTH_SHORT).show();
                UnZipZip();
            }
            if(Utils.getType() == 1)
            {
                showMessage("Подтвердите установку");
                InstallAPK("launcher");
            }
        }
    }

    private class UnZipTask extends AsyncTask <String, Integer, String> {
        private Context context;

        public UnZipTask(Context context) {
            this.context = context;
        }
        @Override
        protected String doInBackground(String... params) {
            String filePath = params[0];
            String destinationPath = params[1];
            try {
                File outdir = new File(destinationPath);
                if(outdir.exists()) outdir.delete();
                if(!outdir.exists()) {
                    outdir.mkdirs();
                    outdir.createNewFile();
                }
                File file = new File(filePath);
                FileInputStream fin = new FileInputStream(filePath);
                ZipInputStream zin = new ZipInputStream(fin, Charset.forName("windows-1251"));
                ZipEntry entry = null;
                String name, dir;
                while ((entry = zin.getNextEntry()) != null) {
                    name = entry.getName();
                    if (entry.isDirectory()) {
                        mkdirs(outdir, name);
                        continue;
                    }
                    dir = dirpart(name);
                    if (dir != null)
                        mkdirs(outdir, dir);
                    byte[] buffer = new byte[4096];
                    java.io.BufferedOutputStream out = new java.io.BufferedOutputStream(new java.io.FileOutputStream(new java.io.File(outdir, name)));
                    int count = -1;
                    int publishprogress = 0;
                    long length = file.length();
                    while ((count = zin.read(buffer)) != -1) {
                        out.write(buffer, 0, count);
                        publishprogress += count;
                        publishProgress((int)(publishprogress / 100/* / length*/));
                    }
                    out.close();
                }
                zin.close();
            } catch (java.io.IOException e) {
                e.printStackTrace();
                writeLog(e);
            }
            return null;
        }

        @Override
        protected void onPreExecute() {
            super.onPreExecute();
            fileName.setVisibility(View.VISIBLE);
            loading.setVisibility(View.VISIBLE);
            loadingPercent.setVisibility(View.VISIBLE);
            progressBar.setVisibility(View.VISIBLE);
            progressBar.setIndeterminate(false);
            progressBar.setMax(100);
        }

        @Override
        protected void onProgressUpdate(Integer... progress) {
            super.onProgressUpdate(progress);
            loading.setText("Идет распаковка файлов...");
            loadingPercent.setText(progress[0] + "%");
            fileName.setText("Пожалуйста подождите");
            progressBar.setProgress(progress[0]);
        }

        @Override
        protected void onPostExecute(String result) {
            fileName.setVisibility(View.INVISIBLE);
            loading.setVisibility(View.INVISIBLE);
            loadingPercent.setVisibility(View.INVISIBLE);
            progressBar.setVisibility(View.INVISIBLE);
            if (result != null)
                Toast.makeText(context, "Ошибка распаковки: " + result, LENGTH_LONG).show();
            else
                Toast.makeText(context, "Файлы распакованы.", Toast.LENGTH_SHORT).show();

            Toast.makeText(context, "Игра установлена. Игра запускается....", Toast.LENGTH_SHORT).show();
            StartGame();
        }

        private void mkdirs(java.io.File outdir, String path)
        {
            java.io.File d = new java.io.File(outdir, path);
            if(!d.exists())
                d.mkdirs();
        }

        private String dirpart(String name)
        {
            int s = name.lastIndexOf(java.io.File.separatorChar);
            return s == -1 ? null : name.substring(0, s);
        }
    }
}
