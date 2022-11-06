//package com.liverussia.launcher.activity;
//
//import android.annotation.SuppressLint;
//import android.app.Dialog;
//import android.content.Intent;
//import android.graphics.Color;
//import android.os.Bundle;
//import android.view.View;
//import android.view.Window;
//import android.webkit.CookieManager;
//import android.webkit.WebView;
//import android.webkit.WebViewClient;
//
//import androidx.appcompat.app.AppCompatActivity;
//
//import com.google.android.gms.safetynet.SafetyNet;
//import com.liverussia.cr.R;
//import com.liverussia.launcher.config.Config;
//
//public class ReCaptchaActivity extends AppCompatActivity {
//    WebView webView;
//    @SuppressLint("ResourceAsColor")
//    @Override
//    protected void onCreate(Bundle savedInstanceState)
//    {
//        super.onCreate(savedInstanceState);
//        setContentView(R.layout.re_captcha_activity);
//        webView = findViewById(R.id.webView);
//        if(savedInstanceState != null) {
//            webView.restoreState(savedInstanceState);
//        } else {
////
//            webView.getSettings().setJavaScriptEnabled(true);
//            webView.getSettings().setDomStorageEnabled(true);
//            webView.setScrollBarStyle(0);
//            webView.getSettings().setUseWideViewPort(false);
//            webView.getSettings().setLoadWithOverviewMode(true);
////            webView.setBackgroundColor(Color.argb(1, 255, 255, 255));
//            webView.loadUrl(Config.URL_RE_CAPTCHA);
////            webView.loadUrl("https://api.blackrussia.online/captcha.html");
////            webView.getSettings().setSupportZoom(true);
//            //webView.getSettings().setBuiltInZoomControls(true);
////            webView.getSettings().setLoadWithOverviewMode(true);
////            webView.getSettings().setUseWideViewPort(true);
////            webView.setWebViewClient(new MyWebViewClient());
////            webView.setScrollBarStyle(View.SCROLLBARS_INSIDE_OVERLAY);
////            webView.setBackgroundColor(0xFFFFFFFF);
//
//
//
//
//
////TODO попытки сделать капчу
////            final Dialog custon_dialog = new Dialog(fragment.getActivity());
////            custon_dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
////            custon_dialog.setContentView(R.layout.re_captcha_activity);
////            custon_dialog.setCancelable(true);
////// custon_dialog.setTitle(null);
////
////            WebView mwebview = (WebView) custon_dialog.findViewById(R.id.webView);
////            mwebview.getSettings().setJavaScriptEnabled(true);
////            mwebview.getSettings().setDomStorageEnabled(true);
////            mwebview.setScrollBarStyle(0);
////            mwebview.getSettings().setUseWideViewPort(false);
////            mwebview.getSettings().setLoadWithOverviewMode(true);
////            mwebview.setBackgroundColor(0x00000000);
//////            mwebview.loadData(Config.URL_RE_CAPTCHA, "text/html", "utf-8");
////            mwebview.loadUrl(Config.URL_RE_CAPTCHA);
////            custon_dialog.show();
////
////
////
////
////
////
////            Intent billingIntent = new Intent(this.getContext(), ReCaptchaActivity.class);
////            startActivity(billingIntent);
////
////
////            SafetyNet.getClient(fragment.getActivity())
////                    .verifyWithRecaptcha(Config.CAPTCHA_SITE_KEY)
////                    .addOnSuccessListener(this::performCaptchaSuccessAction)
////                    .addOnFailureListener(this::performCaptchaFailureAction);
//
//
//
//        }
//    }
//    public class MyWebViewClient extends WebViewClient {
//        @Override
//        public boolean shouldOverrideUrlLoading(WebView view, String url) {
//            view.loadUrl(url);
//            CookieManager.getInstance().setAcceptCookie(true);
//            return true;
//        }
//    }
//}
