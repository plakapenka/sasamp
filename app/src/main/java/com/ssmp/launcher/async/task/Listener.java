package com.ssmp.launcher.async.task;

import com.ssmp.launcher.async.domain.AsyncTaskResult;

public interface Listener<Result> {
    void onAsyncFinished(AsyncTaskResult<Result> data);
}
