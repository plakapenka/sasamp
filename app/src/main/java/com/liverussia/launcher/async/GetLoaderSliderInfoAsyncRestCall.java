package com.liverussia.launcher.async;

import android.app.Activity;
import android.os.AsyncTask;
import android.util.Log;

import com.liverussia.launcher.async.domain.AsyncTaskResult;
import com.liverussia.launcher.async.listener.OnAsyncCriticalErrorListener;
import com.liverussia.launcher.async.listener.OnAsyncSuccessListenerWithResponse;
import com.liverussia.launcher.dto.response.LoaderSliderInfoResponseDto;
import com.liverussia.launcher.dto.response.SpinRouletteResponseDto;
import com.liverussia.launcher.error.apiException.ApiException;
import com.liverussia.launcher.error.apiException.ErrorContainer;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.service.LoaderService;
import com.liverussia.launcher.service.RouletteService;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;
import com.liverussia.launcher.service.impl.LoaderServiceImpl;
import com.liverussia.launcher.service.impl.RouletteServiceImpl;

import org.apache.commons.lang3.StringUtils;
import org.springframework.web.client.ResourceAccessException;

import java.util.Optional;

public class GetLoaderSliderInfoAsyncRestCall extends BackgroundTask<Void, Void, AsyncTaskResult<LoaderSliderInfoResponseDto>> {
    private final Activity mainActivity;
    private final LoaderService loaderService;
    private final ActivityService activityService;

    private OnAsyncSuccessListenerWithResponse<LoaderSliderInfoResponseDto> onAsyncSuccessListener;
    private OnAsyncCriticalErrorListener onAsyncCriticalErrorListener;

    public GetLoaderSliderInfoAsyncRestCall(Activity mainActivity) {
        super();
        this.mainActivity = mainActivity;
        this.loaderService = new LoaderServiceImpl(mainActivity);
    }

    {
        activityService = new ActivityServiceImpl();
    }

    @Override
    protected AsyncTaskResult<LoaderSliderInfoResponseDto> doInBackground(Void... voids) {
        try {
            LoaderSliderInfoResponseDto spinRouletteResponseDto = loaderService.getSliderInfo();
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
    protected void onPostExecute(AsyncTaskResult<LoaderSliderInfoResponseDto> asyncTaskResult) {
        if (asyncTaskResult.getException() != null) {
            ApiException apiException = asyncTaskResult.getException();
            showErrorMessage(apiException);
            onAsyncErrorDo();
//        } else if (isCancelled()) {
//            //TODO разобратсья что это и убрать или исправить
//            Log.d("AsyncRestCall", "canceled");
        } else if (onAsyncSuccessListener != null) {
            onAsyncSuccessListener.onSuccess(asyncTaskResult.getResult());
        }
    }

    private void onAsyncErrorDo() {
        if (onAsyncCriticalErrorListener != null) {
            onAsyncCriticalErrorListener.onCriticalError();
        }
    }

    private void showErrorMessage(ApiException exception) {
        String errorMessage = Optional.ofNullable(exception)
                .map(ApiException::getError)
                .map(ErrorContainer::getMessage)
                .orElse(StringUtils.EMPTY);

        activityService.showMessage(errorMessage, mainActivity);
    }

    public void setOnAsyncSuccessListener(OnAsyncSuccessListenerWithResponse<LoaderSliderInfoResponseDto> onClickListener) {
        this.onAsyncSuccessListener = onClickListener;
    }

    public void setOnAsyncCriticalErrorListener(OnAsyncCriticalErrorListener onAsyncCriticalErrorListener) {
        this.onAsyncCriticalErrorListener = onAsyncCriticalErrorListener;
    }


}
