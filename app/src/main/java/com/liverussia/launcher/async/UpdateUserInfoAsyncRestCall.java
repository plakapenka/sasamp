package com.liverussia.launcher.async;

import android.app.Activity;
import android.os.AsyncTask;
import android.util.Log;

import androidx.appcompat.app.AppCompatActivity;

import com.liverussia.launcher.dto.response.UserInfoDto;
import com.liverussia.launcher.error.apiException.ApiException;
import com.liverussia.launcher.error.apiException.ErrorContainer;
import com.liverussia.launcher.async.domain.AsyncTaskResult;
import com.liverussia.launcher.async.listener.OnAsyncCriticalErrorListener;
import com.liverussia.launcher.async.listener.OnAsyncNotCriticalErrorListener;
import com.liverussia.launcher.async.listener.OnAsyncSuccessListener;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.service.UserService;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;
import com.liverussia.launcher.service.impl.UserServiceImpl;

import org.apache.commons.lang3.StringUtils;
import org.springframework.web.client.ResourceAccessException;

import java.util.HashSet;
import java.util.Optional;
import java.util.Set;

public class UpdateUserInfoAsyncRestCall extends AsyncTask<Void, Void, AsyncTaskResult<UserInfoDto>> {

    private final static Set<ErrorContainer> CRITICAL_ERRORS = new HashSet<>();

    private final Activity mainActivity;
    private final UserService userService;
    private final ActivityService activityService;

    private OnAsyncSuccessListener onAsyncSuccessListener;
    private OnAsyncCriticalErrorListener onAsyncCriticalErrorListener;
    private OnAsyncNotCriticalErrorListener onAsyncNotCriticalErrorListener;

    public UpdateUserInfoAsyncRestCall(Activity mainActivity) {
        this.mainActivity = mainActivity;
        this.userService = new UserServiceImpl(mainActivity);
    }

    {
        activityService = new ActivityServiceImpl();
    }

    static {
        CRITICAL_ERRORS.add(ErrorContainer.AUTHENTICATION_ERROR);
    }

    @Override
    protected AsyncTaskResult<UserInfoDto> doInBackground(Void... voids) {
        try {
            UserInfoDto authenticationResponseDto = userService.updateUserInfo();
            return new AsyncTaskResult<>(authenticationResponseDto);
        } catch (ApiException exception) {
            return new AsyncTaskResult<>(exception);
        } catch (ResourceAccessException exception) {
            return new AsyncTaskResult<>(new ApiException(ErrorContainer.SERVER_CONNECT_ERROR));
        } catch (Exception exception) {
            return new AsyncTaskResult<>(new ApiException(ErrorContainer.OTHER));
        }
    }

    @Override
    protected void onPostExecute(AsyncTaskResult<UserInfoDto> asyncTaskResult) {
        if (asyncTaskResult.getException() != null ) {
            ApiException apiException = asyncTaskResult.getException();
            showErrorMessage(asyncTaskResult.getException());
            validateError(apiException.getError());
        } else if (isCancelled()) {
            //TODO разобратсья что это и убрать или исправить
            Log.d("AsyncRestCall", "canceled");
        } else if (onAsyncSuccessListener != null){
            onAsyncSuccessListener.onSuccess();
        }
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

    private void showErrorMessage(ApiException exception) {
        String errorMessage = Optional.ofNullable(exception)
                .map(ApiException::getError)
                .map(ErrorContainer::getMessage)
                .orElse(StringUtils.EMPTY);

        activityService.showMessage(errorMessage, mainActivity);
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
