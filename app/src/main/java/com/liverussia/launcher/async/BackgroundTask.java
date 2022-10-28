package com.liverussia.launcher.async;

import android.os.Handler;
import android.os.Looper;

public abstract class BackgroundTask<Params, Progress, Result> extends Thread{

    private Handler handler;
    private Params[] params;

    public BackgroundTask() {
        handler = new Handler(Looper.getMainLooper());
    }

    public void execute(Params... params){
        this.params = params;
        start();
    }

    protected abstract Result doInBackground(Params... params);
    protected abstract void onPostExecute(Result result);

    protected void onPreExecute() {
    }

    protected void onProgressUpdate(Progress... values) {
    }

    protected void publishProgress(Progress... values) {
        handler.post(() -> onProgressUpdate(values));
    }

    @Override
    public void run() {
        handler.post(this::onPreExecute);

        Result result = null;

        try {
            result = doInBackground(params);
        } finally {
            Result finalResult = result;
            handler.post(() -> onPostExecute(finalResult));
        }
    }

}
