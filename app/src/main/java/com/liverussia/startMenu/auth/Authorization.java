package com.liverussia.startMenu.auth;

import static com.liverussia.cr.core.Samp.activity;

import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.TextView;
import android.widget.Toast;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.google.android.material.button.MaterialButton;
import com.google.android.material.progressindicator.CircularProgressIndicator;
import com.google.android.material.textfield.TextInputLayout;
import com.liverussia.cr.R;
import com.liverussia.launcher.async.dto.response.LoginGameResponse;
import com.liverussia.launcher.async.service.NetworkService;
import com.liverussia.launcher.storage.Storage;
import com.liverussia.launcher.utils.MainUtils;
import com.liverussia.startMenu.GameMenuStart;
import com.liverussia.startMenu.chooseChar.Transfer;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class Authorization
{
    GameMenuStart gameMenuStart;
    Registration registration;

    public Authorization(GameMenuStart gameMenuStart)
    {
        this.gameMenuStart = gameMenuStart;

        // автологин
        if (Storage.getString("password") != null) {
            logIn();
        }else {
            toggle(true);
        }

        // Кнопка войти
        MaterialButton login_loginButt = activity.findViewById(R.id.login_loginButt);
        login_loginButt.setOnClickListener(view -> {
            TextInputLayout email = activity.findViewById(R.id.login_login);
            Storage.setProperty(activity, "email", email.getEditText().getText().toString());

            TextInputLayout password = activity.findViewById(R.id.login_password);
            Storage.setProperty(activity, "password", password.getEditText().getText().toString());

            logIn();
        });

        // Кнопка зарегистрироваться
        TextView auth_regButt = activity.findViewById(R.id.auth_regButt);
        auth_regButt.setOnClickListener(view -> {
            registration = new Registration(this);
        });

    }

    void logIn()
    {
        toggleContent(false); // показать индикатор загрузки

        NetworkService networkService = MainUtils.retrofit.create(NetworkService.class);

        String email = Storage.getString("email");
        String password = Storage.getString("password");
        Call<LoginGameResponse> call = networkService.gameLogin(email, password);

        call.enqueue(new Callback<LoginGameResponse>() {
            @Override
            public void onResponse(Call<LoginGameResponse> call, Response<LoginGameResponse> response) {
                if (response.isSuccessful() && response.body() != null)
                {// авторизация прошла успешно. закрываем окно

                    Storage.setString( "password", response.body().password);
                    Storage.setString( "email", response.body().email);
                    Storage.setInt("showTestServ", response.body().showTestServ);

                    toggle(false);
                    return;
                }
                toggleContent(true);
                Toast.makeText(activity, "Пароль неверный", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onFailure(Call<LoginGameResponse> call, Throwable t) {
                toggleContent(true);
                Toast.makeText(activity, "Пароль неверный", Toast.LENGTH_SHORT).show();
            }
        });
    }

    public void toggle(boolean toggle) {
        activity.runOnUiThread(() -> {
            final ConstraintLayout auth_layout = activity.findViewById(R.id.auth_layout);
            final CircularProgressIndicator auth_process = activity.findViewById(R.id.auth_process);
            if (toggle) {
                auth_layout.setVisibility(View.VISIBLE);
              //  auth_layout.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.show_from_left));
            } else {
                auth_layout.setVisibility(View.GONE);
                auth_process.setVisibility(View.GONE);
              //  auth_layout.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.hide_to_left));

                gameMenuStart.onCloseAuthorization();
            }
        });
    }

    void toggleContent(boolean toggle)
    {
        activity.runOnUiThread(() -> {
            final CircularProgressIndicator auth_process = activity.findViewById(R.id.auth_process);
            final ConstraintLayout auth_layout = activity.findViewById(R.id.auth_layout);

            if (toggle) {
                auth_process.setVisibility(View.GONE);

                auth_layout.setVisibility(View.VISIBLE);
                auth_layout.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.show_from_bott));
            } else {
                auth_process.setVisibility(View.VISIBLE);

                auth_layout.setVisibility(View.GONE);
                auth_layout.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.hide_to_bott));

              //  gameMenuStart.onCloseAuthorization();
            }
        });
    }

    public void onCloseRegistration()
    {
        registration = null;

        if(Storage.getString("email") != null)
            logIn();
    }
}
