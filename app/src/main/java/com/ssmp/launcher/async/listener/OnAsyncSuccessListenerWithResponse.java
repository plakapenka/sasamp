package com.ssmp.launcher.async.listener;

public interface OnAsyncSuccessListenerWithResponse<Response> {
    void onSuccess(Response response);
}
