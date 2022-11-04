package com.liverussia.launcher.activity.dialogs;

import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.EditText;
import android.widget.TextView;

import androidx.appcompat.app.AlertDialog;

import com.google.android.material.dialog.MaterialAlertDialogBuilder;
import com.liverussia.cr.R;
import com.liverussia.launcher.config.Config;
import com.liverussia.launcher.enums.NativeStorageElements;
import com.liverussia.launcher.fragment.SettingsFragment;
import com.liverussia.launcher.storage.NativeStorage;
import com.liverussia.launcher.utils.Validator;

public class ReCaptchaDialog extends MaterialAlertDialogBuilder implements View.OnClickListener {

    private final AuthenticationDialog authenticationDialog;

    private AlertDialog dialog;
    private EditText nicknameInput;
    private Animation animation;

    public ReCaptchaDialog(AuthenticationDialog authenticationDialog) {
        super(authenticationDialog.getActivity());
        this.authenticationDialog = authenticationDialog;
        createDialog();
    }

    private void createDialog() {
        WebView webView = (WebView) LayoutInflater.from(authenticationDialog.getActivity()).inflate(R.layout.re_captcha_activity, null);

//        WebView webView = view.findViewById(R.id.webView);

        webView.getSettings().setJavaScriptEnabled(true);
        webView.getSettings().setDomStorageEnabled(true);
        webView.setScrollBarStyle(0);
        webView.getSettings().setUseWideViewPort(false);
        webView.getSettings().setLoadWithOverviewMode(true);
        webView.loadUrl(Config.URL_RE_CAPTCHA);

        webView.setWebViewClient(new WebViewClient()
        {
            public boolean shouldOverrideUrlLoading(WebView view, String url)
            {
                view.loadUrl(url);

                return true;
            }
        });

        setView(webView);

        dialog = show();
        dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        setOnDialogCloseListener();
        animation = AnimationUtils.loadAnimation(getContext(), R.anim.button_click);
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
}
