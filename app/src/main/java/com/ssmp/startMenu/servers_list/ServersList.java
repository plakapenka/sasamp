package com.ssmp.startMenu.servers_list;

import static com.ssmp.cr.core.Samp.activity;

import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;
import android.widget.Toast;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.ssmp.launcher.storage.Storage;
import com.ssmp.startMenu.GameMenuStart;
import com.ssmp.cr.R;
import com.ssmp.launcher.async.service.NetworkService;
import com.ssmp.launcher.utils.MainUtils;

import java.util.ArrayList;
import java.util.List;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class ServersList {
    GameMenuStart gameMenuStart;

    public ServersList(GameMenuStart gameMenuStart)
    {
        this.gameMenuStart = gameMenuStart;

        ImageView backButt = activity.findViewById(R.id.select_server_back);
        backButt.setOnClickListener(view -> {
            toggle(false);
        });

        loadServersList();
        toggle(true);
    }

    void loadServersList()
    {
        NetworkService networkService = MainUtils.retrofit.create(NetworkService.class);

        Call<List<ServerData>> call = networkService.getServersList();

        call.enqueue(new Callback<List<ServerData>>() {
            @Override
            public void onResponse(Call<List<ServerData>> call, Response<List<ServerData>> response)
            {
                List<ServerData> temp = new ArrayList<>();
                if (response.isSuccessful() && response.body() != null)
                {
                    if(Storage.getInt("showTestServ") != 1) {
                        for (ServerData serverData : response.body()) {
                            if(serverData.locked == 0)
                                temp.add(serverData);
                        }
                    } else {
                      temp.addAll(response.body());
                    }

                    RecyclerView select_server_recycle = activity.findViewById(R.id.select_server_recycle);
                    select_server_recycle.setLayoutManager(new GridLayoutManager(activity, 4));

                    ServersAdapter serversAdapter = new ServersAdapter(activity, temp, ServersList.this);
                    select_server_recycle.setAdapter(serversAdapter);
                    return;
                }
                Toast.makeText(activity, "Неудалось получить список серверов", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onFailure(Call<List<ServerData>> call, Throwable t) {
                Toast.makeText(activity, "Неудалось получить список серверов", Toast.LENGTH_SHORT).show();
            }
        });
    }

    public void toggle(boolean toggle)
    {
        activity.runOnUiThread(()-> {
            final ConstraintLayout select_server_main_layout = activity.findViewById(R.id.select_server_main_layout);

            if(toggle){
                select_server_main_layout.setVisibility(View.VISIBLE);
                select_server_main_layout.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.show_from_left));
            }
            else {
                select_server_main_layout.setVisibility(View.GONE);
                select_server_main_layout.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.hide_to_left));

                gameMenuStart.onCloseChooseServer();
            }
        });
    }
}
