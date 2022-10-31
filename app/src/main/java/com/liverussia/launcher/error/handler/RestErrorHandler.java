package com.liverussia.launcher.error.handler;

import android.util.Log;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.liverussia.launcher.error.apiException.ApiError;
import com.liverussia.launcher.error.apiException.ApiException;
import com.liverussia.launcher.error.apiException.ErrorContainer;

import org.springframework.http.HttpStatus;
import org.springframework.http.client.ClientHttpResponse;
import org.springframework.web.client.ResponseErrorHandler;

import java.io.IOException;

public class RestErrorHandler implements ResponseErrorHandler {

    private final ObjectMapper objectMapper;

    {
        objectMapper = new ObjectMapper();
    }

    @Override
    public boolean hasError(ClientHttpResponse response) throws IOException {
        return !HttpStatus.OK.equals(response.getStatusCode());
    }

    @Override
    public void handleError(ClientHttpResponse response) throws IOException {

        ApiError apiError = getMessageFromResponse(response);
        if (apiError == null) {
            throw new ApiException(ErrorContainer.OTHER);
        } else {
            throw getApiExceptionFromResponse(apiError);
        }
    }

    private ApiException getApiExceptionFromResponse(ApiError apiError) {
        return new ApiException(ErrorContainer.of(apiError.getErrorCode()));
    }

    private ApiError getMessageFromResponse(ClientHttpResponse response) {
        ApiError apiError = null;
        try {
          if (response.getBody() != null) {
              apiError = objectMapper.readValue(response.getBody(), ApiError.class);
          }
        } catch (IOException exception) {
            Log.d("handlerError: ", "Unable to build response from external service");
        }

        return apiError;
    }
}
