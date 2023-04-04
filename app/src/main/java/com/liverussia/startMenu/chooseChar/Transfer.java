package com.liverussia.startMenu.chooseChar;

import static com.liverussia.cr.core.Samp.activity;

import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.RadioButton;
import android.widget.Toast;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.google.android.material.button.MaterialButton;
import com.liverussia.cr.R;
import com.liverussia.launcher.async.service.NetworkService;
import com.liverussia.launcher.storage.Storage;
import com.liverussia.launcher.utils.MainUtils;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class Transfer {
    CharsList charsList;

    public Transfer(CharsList charsList) {
        this.charsList = charsList;

        toggle(true);

        MaterialButton reg_char_old_reg_butt = activity.findViewById(R.id.reg_char_old_reg_butt);
        reg_char_old_reg_butt.setOnClickListener(view -> {
            sendTransfer();
        });

        ImageView reg_char_old_back = activity.findViewById(R.id.reg_char_old_back);
        reg_char_old_back.setOnClickListener(view -> {
            toggle(false);
        });
    }

    public void sendTransfer() {
        NetworkService networkService = MainUtils.retrofit.create(NetworkService.class);

        EditText nick = activity.findViewById(R.id.reg_char_old_nick);
        EditText pass = activity.findViewById(R.id.reg_char_old_pass);

        Call<String> call = networkService.transferChar(
                Storage.getInt("selectedServerId"),
                Storage.getString("email"),
                Storage.getString("password"),
                nick.getText().toString(),
                pass.getText().toString()
        );

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
                    Toast.makeText(activity, "Персонаж зарегистрирован!", Toast.LENGTH_SHORT).show();
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
            final ConstraintLayout reg_char_old_main_layout = activity.findViewById(R.id.reg_char_old_main_layout);

            if (toggle) {
                reg_char_old_main_layout.setVisibility(View.VISIBLE);
                reg_char_old_main_layout.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.show_from_left));
            } else {
                reg_char_old_main_layout.setVisibility(View.GONE);
                reg_char_old_main_layout.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.hide_to_left));

                charsList.onCloseTransfer();
            }
        });
    }
}
