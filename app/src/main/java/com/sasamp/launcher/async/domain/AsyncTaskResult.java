package com.sasamp.launcher.async.domain;

import com.sasamp.launcher.error.apiException.ApiException;

import lombok.Data;

@Data
public class AsyncTaskResult<Result> {
    private Result result;
    private ApiException exception;

    public AsyncTaskResult(Result result) {
        super();
        this.result = result;
    }

    public AsyncTaskResult(ApiException exception) {
        super();
        this.exception = exception;
    }
}