package com.liverussia.launcher.async;

import android.app.Activity;
import android.os.AsyncTask;
import android.util.Log;

import com.liverussia.launcher.dto.request.RefreshTokenRequestDto;
import com.liverussia.launcher.dto.response.AuthenticationResponseDto;
import com.liverussia.launcher.dto.response.UserInfoDto;
import com.liverussia.launcher.enums.StorageElements;
import com.liverussia.launcher.error.ErrorUtils;
import com.liverussia.launcher.error.apiException.ApiException;
import com.liverussia.launcher.error.apiException.ErrorContainer;
import com.liverussia.launcher.async.domain.AsyncTaskResult;
import com.liverussia.launcher.async.listener.OnAsyncCriticalErrorListener;
import com.liverussia.launcher.async.listener.OnAsyncNotCriticalErrorListener;
import com.liverussia.launcher.async.listener.OnAsyncSuccessListener;
import com.liverussia.launcher.other.NetworkService;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.service.AuthenticationService;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;
import com.liverussia.launcher.service.impl.AuthenticationServiceImpl;
import com.liverussia.launcher.storage.Storage;
import com.liverussia.launcher.utils.DateTimeUtils;

import org.apache.commons.lang3.StringUtils;
import org.springframework.web.client.ResourceAccessException;

import java.net.ConnectException;
import java.util.HashSet;
import java.util.Optional;
import java.util.Set;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

import static com.liverussia.launcher.config.Config.LAUNCHER_SERVER_URI;

public class UpdateTokensAsyncRestCall {
    private final static Set<ErrorContainer> CRITICAL_ERRORS = new HashSet<>();

    private final Activity activity;
    private final AuthenticationService authenticationService;
    private final ActivityService activityService;
    private final Retrofit retrofit;

    private OnAsyncSuccessListener onAsyncSuccessListener;
    private OnAsyncCriticalErrorListener onAsyncCriticalErrorListener;
    private OnAsyncNotCriticalErrorListener onAsyncNotCriticalErrorListener;

    public UpdateTokensAsyncRestCall(Activity activity) {
        this.activity = activity;
        this.authenticationService = new AuthenticationServiceImpl(activity);
    }

    {
        activityService = new ActivityServiceImpl();

        retrofit = new Retrofit.Builder()
                .baseUrl(LAUNCHER_SERVER_URI)
                .addConverterFactory(GsonConverterFactory.create())
                .build();
    }

    static {
        CRITICAL_ERRORS.add(ErrorContainer.AUTHENTICATION_ERROR);
    }

    public void refreshTokens(boolean withErrorMessage) {
        RefreshTokenRequestDto refreshTokenRequestDto = authenticationService.getRefreshTokenRequestDto();

        NetworkService networkService = retrofit.create(NetworkService.class);
        Call<AuthenticationResponseDto> call = networkService.refreshTokens(refreshTokenRequestDto);

        call.enqueue(new Callback<AuthenticationResponseDto>() {
            @Override
            public void onResponse(Call<AuthenticationResponseDto> call, Response<AuthenticationResponseDto> response) {
                if (response.isSuccessful()) {
                    authenticationService.saveUserInfoIntoStorage(response.body());
                    onAsyncSuccessListener.onSuccess();
                } else {
                    ErrorContainer error = ErrorUtils.parseError(response, retrofit);
                    if (withErrorMessage) {
                        showErrorMessage(error);
                    }
                    validateError(error);
                }
            }

            @Override
            public void onFailure(Call<AuthenticationResponseDto> call, Throwable throwable) {
                ErrorContainer error;

                if (throwable instanceof ResourceAccessException || throwable instanceof ConnectException) {
                    error = ErrorContainer.SERVER_CONNECT_ERROR;
                } else {
                    error = ErrorContainer.OTHER;
                }

                if (withErrorMessage) {
                    showErrorMessage(error);
                }
                validateError(error);
            }
        });
    }

    private void validateError(ErrorContainer error) {
        if (CRITICAL_ERRORS.contains(error) && onAsyncCriticalErrorListener != null) {
            onAsyncCriticalErrorListener.onCriticalError();
            return;
        }

        if (onAsyncNotCriticalErrorListener != null) {
            onAsyncNotCriticalErrorListener.onNotCriticalError();
        }
    }

    private void showErrorMessage(ErrorContainer error) {
        String errorMessage = Optional.ofNullable(error)
                .map(ErrorContainer::getMessage)
                .orElse(StringUtils.EMPTY);

        activityService.showMessage(errorMessage, activity);
    }

    public void setOnAsyncSuccessListener(OnAsyncSuccessListener onClickListener) {
        this.onAsyncSuccessListener = onClickListener;
    }

    public void setOnAsyncCriticalErrorListener(OnAsyncCriticalErrorListener onAsyncCriticalErrorListener) {
        this.onAsyncCriticalErrorListener = onAsyncCriticalErrorListener;
    }

    public void setOnAsyncNotCriticalErrorListener(OnAsyncNotCriticalErrorListener onAsyncNotCriticalErrorListener) {
        this.onAsyncNotCriticalErrorListener = onAsyncNotCriticalErrorListener;
    }
}
