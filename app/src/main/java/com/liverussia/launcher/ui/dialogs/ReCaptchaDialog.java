package com.liverussia.launcher.ui.dialogs;

import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.view.LayoutInflater;
import android.view.View;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.FragmentActivity;

import com.google.android.material.dialog.MaterialAlertDialogBuilder;
import com.liverussia.cr.R;
import com.liverussia.launcher.config.Config;
import com.liverussia.launcher.ui.dialogs.listener.OnDialogCloseListener;
import com.liverussia.launcher.web.ReCaptchaWebInterface;

public class ReCaptchaDialog extends MaterialAlertDialogBuilder implements View.OnClickListener {

    private final FragmentActivity activity;
    private AlertDialog dialog;

    private OnDialogCloseListener<String> onDialogCloseListener;

    public ReCaptchaDialog(FragmentActivity activity) {
        super(activity);
        this.activity = activity;
    }

    public void createDialog() {
        WebView webView = (WebView) LayoutInflater.from(activity).inflate(R.layout.re_captcha_activity, null);

        webView.getSettings().setJavaScriptEnabled(true);
        webView.getSettings().setDomStorageEnabled(true);
        webView.setScrollBarStyle(0);
        webView.getSettings().setUseWideViewPort(false);
        webView.getSettings().setLoadWithOverviewMode(true);
        webView.addJavascriptInterface(new ReCaptchaWebInterface(activity, this), "android");
        webView.loadUrl(Config.URL_RE_CAPTCHA);

        webView.setWebViewClient(new WebViewClient() {
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                view.loadUrl(url);
                return true;
            }
        });

        setView(webView);

        dialog = show();
        dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        setOnDialogCloseListener();
    }

    private void setOnDialogCloseListener() {
        dialog.setOnDismissListener(alertDialog -> {

        });
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            default:
                break;
        }
    }

    public void performCaptchaPassAction(String token) {
        dialog.dismiss();
        onDialogCloseListener.onClose(token);
    }


    public void setOnDialogCloseListener(OnDialogCloseListener<String> onDialogCloseListener) {
        this.onDialogCloseListener = onDialogCloseListener;
    }
}
