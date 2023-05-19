package com.sasamp.startMenu;

import android.app.Activity;
import android.view.View;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.google.android.material.button.MaterialButton;
import com.sasamp.cr.R;
import com.sasamp.launcher.storage.Storage;
import com.sasamp.startMenu.servers_list.ServersList;

public class GameMenuStart {
    Activity activity;
    TextView login_change_server;

    ServersList serversList;

    MaterialButton game_menu_start_play_butt;
    ConstraintLayout menu_layout;

    native void connectToServer(String nick, String ip, int port);

    public GameMenuStart(Activity activity) {
        this.activity = activity;

        menu_layout = activity.findViewById(R.id.menu_layout);

        login_change_server = activity.findViewById(R.id.login_change_server);
        login_change_server.setOnClickListener(view -> {
            serversList = new ServersList(this);
        });

//        if (Storage.getString("selectedServerName") != null) {
//            final String servName = Storage.getString( "selectedServerName");
//            login_change_server.setText( String.format("Сервер: %s", servName) );
//        }
//        else {
//            serversList = new ServersList(this);
//        }

        game_menu_start_play_butt = activity.findViewById(R.id.game_menu_start_play_butt);
        game_menu_start_play_butt.setOnClickListener(view -> {
//            connectToServer(
//                    Storage.getString("nickname"),
//                    Storage.getString("selectedServerIp"),
//                    Storage.getInt("selectedServerPort")
//            );
            activity.findViewById(R.id.startmenu_main_layout).setVisibility(View.GONE);
            connectToServer(
                    "Egor_Plakapen",
                    "revo.samp-rp.ru",
                    7777
            );
//
        });

    }

    public void onCloseChooseServer() {
        serversList = null;

        activity.runOnUiThread(()-> {
            final String servName = Storage.getString("selectedServerName");
            login_change_server.setText( String.format("Сервер: %s", servName) );
        });
    }
}
