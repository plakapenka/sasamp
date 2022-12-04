package com.liverussia.launcher.async.task;

import com.liverussia.launcher.async.domain.AsyncTaskResult;

public interface Listener<Result> {
    void onAsyncFinished(AsyncTaskResult<Result> data);
}
