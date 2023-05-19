package com.sasamp.launcher.async.task;

import com.sasamp.launcher.async.domain.AsyncTaskResult;

public interface Listener<Result> {
    void onAsyncFinished(AsyncTaskResult<Result> data);
}
