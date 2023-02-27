package com.liverussia.launcher.ui.activity;

import android.app.Activity;
import android.os.Bundle;
import com.liverussia.cr.R;
import com.liverussia.launcher.config.Config;
import com.liverussia.launcher.domain.enums.BillingParameters;

import android.annotation.SuppressLint;
import android.view.View;
import android.webkit.CookieManager;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.webkit.WebViewClient;

@SuppressLint("SetJavaScriptEnabled")
public class BillingActivity extends Activity {


//    @SuppressLint("ResourceAsColor")
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.forum_activity);

        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE
                        //   | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);

        WebView webView = findViewById(R.id.webView);
        if(savedInstanceState != null) {
            webView.restoreState(savedInstanceState);
        } else {
            String uri = createUri();

            webView.getSettings().setJavaScriptEnabled(true);
//            webView.getSettings().setSupportZoom(true);
            //webView.getSettings().setBuiltInZoomControls(true);
//            webView.getSettings().setLoadWithOverviewMode(true);
//            webView.getSettings().setUseWideViewPort(true);
            webView.setWebViewClient(new MyWebViewClient());
            webView.setWebChromeClient(new WebChromeClient());
            webView.setScrollBarStyle(View.SCROLLBARS_INSIDE_OVERLAY);

            webView.loadUrl(uri);
//            webView.setBackgroundColor(0xFFFFFFFF);
        }
    }

    private String createUri() {

        Bundle extras = getIntent().getExtras();
        String nickname = extras.getString(BillingParameters.NICKNAME.getName());
        String email = extras.getString(BillingParameters.EMAIL.getName());
        String serverId = extras.getString(BillingParameters.SERVER_SELECTED_ID.getName());
        String serverName = extras.getString(BillingParameters.SERVER_SELECTED_NAME.getName());
        String sum = extras.getString(BillingParameters.DONATE_SUM.getName());
        String captcha = extras.getString(BillingParameters.CAPTCHA.getName());

        return Config.createBillingUri(serverId, serverName, sum, nickname, email, captcha);
    }

    public class MyWebViewClient extends WebViewClient {
        @Override
        public boolean shouldOverrideUrlLoading(WebView view, String url) {
            view.loadUrl(url);
            CookieManager.getInstance().setAcceptCookie(true);
            return true;
        }
    }
}

