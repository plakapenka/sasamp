package com.liverussia.launcher.async;

import android.os.AsyncTask;
import android.util.Log;

import com.liverussia.launcher.activity.MainActivity;
import com.liverussia.launcher.dto.response.ServerImagesResponseDto;
import com.liverussia.launcher.error.apiException.ApiException;
import com.liverussia.launcher.error.apiException.ErrorContainer;
import com.liverussia.launcher.async.domain.AsyncTaskResult;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.service.RouletteService;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;
import com.liverussia.launcher.service.impl.RouletteServiceImpl;

import org.apache.commons.lang3.StringUtils;
import org.springframework.web.client.ResourceAccessException;

import java.util.Optional;

public class GetPossiblePrizesAsyncRestCall extends AsyncTask<Void, Void, AsyncTaskResult<ServerImagesResponseDto>> {

    private final MainActivity mainActivity;
    private final RouletteService rouletteService;
    private final ActivityService activityService;

    public GetPossiblePrizesAsyncRestCall(MainActivity mainActivity) {
        this.mainActivity = mainActivity;
        this.rouletteService = new RouletteServiceImpl(mainActivity);
    }

    {
        activityService = new ActivityServiceImpl();
    }

    @Override
    protected AsyncTaskResult<ServerImagesResponseDto> doInBackground(Void... voids) {
        try {
            ServerImagesResponseDto possiblePrizesInfoResponseDto = rouletteService.getPossiblePrizes();
            return new AsyncTaskResult<>(possiblePrizesInfoResponseDto);
        } catch (ApiException exception) {
            return new AsyncTaskResult<>(exception);
        } catch (ResourceAccessException exception) {
            return new AsyncTaskResult<>(new ApiException(ErrorContainer.SERVER_CONNECT_ERROR));
        } catch (Exception exception) {
            return new AsyncTaskResult<>(new ApiException(ErrorContainer.OTHER));
        }
    }

    @Override
    protected void onPostExecute(AsyncTaskResult<ServerImagesResponseDto> asyncTaskResult) {
        if (asyncTaskResult.getException() != null ) {
            showErrorMessage(asyncTaskResult.getException());
        } else if (isCancelled()) {
            //TODO разобратсья что это и убрать или исправить
            Log.d("AsyncRestCall", "canceled");
        } else {
            mainActivity.addPossiblePrizes(asyncTaskResult.getResult());
        }
    }

    private void showErrorMessage(ApiException exception) {
        String errorMessage = Optional.ofNullable(exception)
                .map(ApiException::getError)
                .map(ErrorContainer::getMessage)
                .orElse(StringUtils.EMPTY);

        activityService.showMessage(errorMessage, mainActivity);
    }
}
