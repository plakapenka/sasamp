package com.liverussia.startMenu.auth;

import static androidx.constraintlayout.helper.widget.MotionEffect.TAG;
import static com.liverussia.cr.core.Samp.activity;

import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.TextView;
import android.widget.Toast;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.google.android.material.button.MaterialButton;
import com.google.android.material.textfield.TextInputEditText;
import com.google.android.material.textfield.TextInputLayout;
import com.liverussia.cr.R;
import com.liverussia.cr.gui.util.Utils;
import com.liverussia.launcher.async.service.NetworkService;
import com.liverussia.launcher.storage.Storage;
import com.liverussia.launcher.utils.MainUtils;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class Registration {
    Authorization authorization;

    public Registration(Authorization authorization)
    {
        this.authorization = authorization;

        toggle(true);

        TextView reg_back_butt = activity.findViewById(R.id.reg_back_butt);
        reg_back_butt.setOnClickListener(view -> {
            toggle(false);
        });

        MaterialButton reg_nextButt = activity.findViewById(R.id.reg_nextButt);
        reg_nextButt.setOnClickListener(view -> {
            TextInputLayout reg_acc_password = activity.findViewById(R.id.reg_acc_password);
            TextInputLayout reg_password_repeat = activity.findViewById(R.id.reg_password_repeat);
            TextInputLayout reg_login = activity.findViewById(R.id.reg_login);

            if(reg_password_repeat.getEditText().getText() == null)
                return;


            if(reg_password_repeat.getEditText().getText().toString().compareTo( reg_acc_password.getEditText().getText().toString() ) != 0) {
                Toast.makeText(activity, "Пароли не совпадают!", Toast.LENGTH_SHORT).show();
                return;
            }
            if ( !Utils.isEmailValid( reg_login.getEditText().getText().toString() )) {
                Toast.makeText(activity, "Неверный email!", Toast.LENGTH_SHORT).show();
                return;
            }
            sendRegister();
        });
    }

    void sendRegister() {
        NetworkService networkService = MainUtils.retrofit.create(NetworkService.class);

        TextInputLayout reg_acc_password = activity.findViewById(R.id.reg_acc_password);
        final String pass = reg_acc_password.getEditText().getText().toString();

        TextInputLayout reg_login = activity.findViewById(R.id.reg_login);
        final String mail = reg_login.getEditText().getText().toString();

        Storage.setString("email", mail);
        Storage.setString("password", pass);

        Call<String> call = networkService.registerAccount(mail, pass);

        call.enqueue(new Callback<String>() {
            @Override
            public void onResponse(Call<String> call, Response<String> response) {

                if (response.body() != null && response.isSuccessful())
                {
                    String result = response.body();
                    if(!result.equals("ok!"))
                    {
                        Toast.makeText(activity, result, Toast.LENGTH_SHORT).show();
                        return;
                    }
                    toggle(false);
                    Toast.makeText(activity, "Аккаунт зарегистрирован!", Toast.LENGTH_SHORT).show();
                    return;
                }


            }

            @Override
            public void onFailure(Call<String> call, Throwable t) {
                Toast.makeText(activity, "Ошибка при регистрации", Toast.LENGTH_SHORT).show();
            }
        });
    }

    public void toggle(boolean toggle) {
        activity.runOnUiThread(() -> {
            final ConstraintLayout reg_layout = activity.findViewById(R.id.reg_layout);

            if (toggle) {
                reg_layout.setVisibility(View.VISIBLE);
             //   reg_layout.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.show_from_left));
            } else {
                reg_layout.setVisibility(View.GONE);
             //   select_char_main_layout.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.hide_to_left));

                authorization.onCloseRegistration();
            }
        });
    }
}
