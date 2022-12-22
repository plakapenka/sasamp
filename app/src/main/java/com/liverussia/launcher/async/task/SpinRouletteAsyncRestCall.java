package com.liverussia.launcher.async.task;

import android.app.Activity;

import com.liverussia.launcher.async.listener.OnAsyncCriticalErrorListener;
import com.liverussia.launcher.async.dto.response.SpinRouletteResponseDto;
import com.liverussia.launcher.domain.enums.StorageElements;
import com.liverussia.launcher.error.ErrorUtils;
import com.liverussia.launcher.error.apiException.ErrorContainer;
import com.liverussia.launcher.async.listener.OnAsyncSuccessListenerWithResponse;
import com.liverussia.launcher.async.service.NetworkService;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;
import com.liverussia.launcher.storage.Storage;

import org.apache.commons.lang3.StringUtils;
import org.springframework.web.client.ResourceAccessException;

import java.net.ConnectException;
import java.util.Optional;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

public class SpinRouletteAsyncRestCall {
    private final Activity activity;
    private final ActivityService activityService;
    private final Retrofit retrofit;

    private OnAsyncSuccessListenerWithResponse<SpinRouletteResponseDto> onAsyncSuccessListener;
    private OnAsyncCriticalErrorListener onAsyncCriticalErrorListener;

    public SpinRouletteAsyncRestCall(Activity activity) {
        this.activity = activity;

        String url = Storage.getProperty(StorageElements.ROULETTE_SERVER_HOST.getValue(), activity);
        this.retrofit = new Retrofit.Builder()
                .baseUrl(url)
                .addConverterFactory(GsonConverterFactory.create())
                .build();
    }

    {
        activityService = new ActivityServiceImpl();
    }

    public void spinRoulette() {
        NetworkService networkService = retrofit.create(NetworkService.class);

        String token = Storage.getProperty(StorageElements.ACCESS_TOKEN.getValue(), activity);
        Call<SpinRouletteResponseDto> call = networkService.spinRoulette(token);


        call.enqueue(new Callback<SpinRouletteResponseDto>() {
            @Override
            public void onResponse(Call<SpinRouletteResponseDto> call, Response<SpinRouletteResponseDto> response) {
                if (response.isSuccessful()) {
                    onAsyncSuccessListener.onSuccess(response.body());
                } else {
                    ErrorContainer error = ErrorUtils.parseError(response, retrofit);
                    showErrorMessage(error);
                    onAsyncCriticalErrorListener.onCriticalError();
                }
            }

            @Override
            public void onFailure(Call<SpinRouletteResponseDto> call, Throwable throwable) {
                ErrorContainer error;

                if (throwable instanceof ResourceAccessException || throwable instanceof ConnectException) {
                    error = ErrorContainer.SERVER_CONNECT_ERROR;
                } else {
                    error = ErrorContainer.OTHER;
                }

                showErrorMessage(error);
                onAsyncCriticalErrorListener.onCriticalError();
            }
        });

    }

    private void showErrorMessage(ErrorContainer error) {
        String errorMessage = Optional.ofNullable(error)
                .map(ErrorContainer::getMessage)
                .orElse(StringUtils.EMPTY);

        activityService.showMessage(errorMessage, activity);
    }

    public void setOnAsyncSuccessListener(OnAsyncSuccessListenerWithResponse<SpinRouletteResponseDto> onClickListener) {
        this.onAsyncSuccessListener = onClickListener;
    }

    public void setOnAsyncCriticalErrorListener(OnAsyncCriticalErrorListener onAsyncCriticalErrorListener) {
        this.onAsyncCriticalErrorListener = onAsyncCriticalErrorListener;
    }
}
