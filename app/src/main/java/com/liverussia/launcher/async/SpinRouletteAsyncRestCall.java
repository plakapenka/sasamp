package com.liverussia.launcher.async;

import android.app.Activity;
import android.os.AsyncTask;
import android.util.Log;

import androidx.appcompat.app.AppCompatActivity;

import com.liverussia.launcher.dto.response.SpinRouletteResponseDto;
import com.liverussia.launcher.error.apiException.ApiException;
import com.liverussia.launcher.error.apiException.ErrorContainer;
import com.liverussia.launcher.async.domain.AsyncTaskResult;
import com.liverussia.launcher.async.listener.OnAsyncSuccessListenerWithResponse;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.service.RouletteService;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;
import com.liverussia.launcher.service.impl.RouletteServiceImpl;

import org.apache.commons.lang3.StringUtils;
import org.springframework.web.client.ResourceAccessException;

import java.util.Optional;

public class SpinRouletteAsyncRestCall extends AsyncTask<Void, Void, AsyncTaskResult<SpinRouletteResponseDto>> {
    private final Activity mainActivity;
    private final RouletteService rouletteService;
    private final ActivityService activityService;

    private OnAsyncSuccessListenerWithResponse<SpinRouletteResponseDto> onAsyncSuccessListener;

    public SpinRouletteAsyncRestCall(Activity mainActivity) {
        this.mainActivity = mainActivity;
        this.rouletteService = new RouletteServiceImpl(mainActivity);
    }

    {
        activityService = new ActivityServiceImpl();
    }

    @Override
    protected AsyncTaskResult<SpinRouletteResponseDto> doInBackground(Void... voids) {
        try {
            SpinRouletteResponseDto spinRouletteResponseDto = rouletteService.spinRoulette();
            return new AsyncTaskResult<>(spinRouletteResponseDto);
        } catch (ApiException exception) {
            return new AsyncTaskResult<>(exception);
        } catch (ResourceAccessException exception) {
            return new AsyncTaskResult<>(new ApiException(ErrorContainer.SERVER_CONNECT_ERROR));
        } catch (Exception exception) {
            return new AsyncTaskResult<>(new ApiException(ErrorContainer.OTHER));
        }
    }

    @Override
    protected void onPostExecute(AsyncTaskResult<SpinRouletteResponseDto> asyncTaskResult) {
        if (asyncTaskResult.getException() != null ) {
            showErrorMessage(asyncTaskResult.getException());
        } else if (isCancelled()) {
            //TODO разобратсья что это и убрать или исправить
            Log.d("AsyncRestCall", "canceled");
        } else if (onAsyncSuccessListener != null) {
            onAsyncSuccessListener.onSuccess(asyncTaskResult.getResult());
        }
    }

    private void showErrorMessage(ApiException exception) {
        String errorMessage = Optional.ofNullable(exception)
                .map(ApiException::getError)
                .map(ErrorContainer::getMessage)
                .orElse(StringUtils.EMPTY);

        activityService.showMessage(errorMessage, mainActivity);
    }

    public void setOnAsyncSuccessListener(OnAsyncSuccessListenerWithResponse<SpinRouletteResponseDto> onClickListener) {
        this.onAsyncSuccessListener = onClickListener;
    }
}
