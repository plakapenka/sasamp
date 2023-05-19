package com.ssmp.startMenu.chooseChar;

import static com.ssmp.cr.core.Samp.activity;

import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;
import android.widget.Toast;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.ssmp.cr.R;
import com.ssmp.launcher.async.service.NetworkService;
import com.ssmp.launcher.storage.Storage;
import com.ssmp.launcher.utils.MainUtils;
import com.ssmp.startMenu.GameMenuStart;

import java.util.ArrayList;
import java.util.List;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class CharsList {
    GameMenuStart gameMenuStart;

    ConstraintLayout select_char_main_layout;
    ImageView select_char_back;

    RegisterChar registerChar;
    Transfer transfer;

    public CharsList(GameMenuStart gameMenuStart) {
        this.gameMenuStart = gameMenuStart;

        select_char_main_layout = activity.findViewById(R.id.select_char_main_layout);
        select_char_main_layout.setVisibility(View.VISIBLE);

        select_char_back = activity.findViewById(R.id.select_char_back);
        select_char_back.setOnClickListener(view -> {
            Storage.remove("nickname");
            toggle(false);
        });

        toggle(true);

        loadCharsList();
    }

    public void loadCharsList() {
        NetworkService networkService = MainUtils.retrofit.create(NetworkService.class);

        String email = Storage.getString("email");
        String password = Storage.getString("password");
        int servId = Storage.getInt("selectedServerId");
        Call<List<CharData>> call = networkService.getChars(email, password, servId);

        call.enqueue(new Callback<List<CharData>>() {
            @Override
            public void onResponse(Call<List<CharData>> call, Response<List<CharData>> response) {
                List<CharData> temp = new ArrayList<>();
                if (response.isSuccessful() && response.body() != null) {
                    temp.addAll(response.body());
                    //  return;
                }
                // Создать
                CharData add = new CharData();
                add.nickname = "";
                add.money = -1;
                add.level = -1;
                temp.add(add);

                // Перенести
                CharData add1 = new CharData();
                add1.nickname = "";
                add1.money = -2;
                add1.level = -2;
                temp.add(add1);

                RecyclerView select_char_recycle = activity.findViewById(R.id.select_char_recycle);
                select_char_recycle.setLayoutManager(new LinearLayoutManager(activity, LinearLayoutManager.HORIZONTAL, false));

                CharsAdapter charsAdapter = new CharsAdapter(activity, temp, CharsList.this);
                select_char_recycle.setAdapter(charsAdapter);

                // Toast.makeText(activity, "Неудалось получить список серверов", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onFailure(Call<List<CharData>> call, Throwable t) {
                Toast.makeText(activity, "Неудалось получить список персонажей", Toast.LENGTH_SHORT).show();
            }
        });
    }

    public void toggle(boolean toggle) {
        activity.runOnUiThread(() -> {
            final ConstraintLayout select_char_main_layout = activity.findViewById(R.id.select_char_main_layout);

            if (toggle) {
                select_char_main_layout.setVisibility(View.VISIBLE);
                select_char_main_layout.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.show_from_left));
            } else {
                select_char_main_layout.setVisibility(View.GONE);
                select_char_main_layout.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.hide_to_left));

                gameMenuStart.onCloseCharsList();
            }
        });
    }

    public void onCreateRegisterChar() {
        registerChar = new RegisterChar(this);
    }

    public void onCloseRegisterChar() {
        registerChar = null;
        loadCharsList();
    }

    public void createTransfer() {
        transfer = new Transfer(this);
    }

    public void onCloseTransfer() {
        transfer = null;
        loadCharsList();
    }
}
