package com.sasamp.launcher.error;

import com.sasamp.launcher.error.apiException.ApiError;
import com.sasamp.launcher.error.apiException.ErrorContainer;

import java.io.IOException;
import java.lang.annotation.Annotation;

import okhttp3.ResponseBody;
import retrofit2.Converter;
import retrofit2.Response;
import retrofit2.Retrofit;

public class ErrorUtils {


    private static int OTHER_ERROR_CODE = 999;

    public static ErrorContainer parseError(Response<?> response, Retrofit retrofit) {
        Converter<ResponseBody, ApiError> converter = retrofit
                        .responseBodyConverter(ApiError.class, new Annotation[0]);

        ApiError error;

        try {
            error = converter.convert(response.errorBody());
        } catch (IOException e) {
            return ErrorContainer.of(OTHER_ERROR_CODE);
        }

        return ErrorContainer.of(error.getErrorCode());
    }
}
