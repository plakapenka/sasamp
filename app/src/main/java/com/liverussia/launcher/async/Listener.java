package com.liverussia.launcher.async;

import com.liverussia.launcher.async.domain.AsyncTaskResult;

public interface Listener<Result> {
    void onAsyncFinished(AsyncTaskResult<Result> data);
}
