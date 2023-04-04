package com.liverussia.startMenu;

import android.app.Activity;
import android.view.View;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.google.android.material.button.MaterialButton;
import com.google.android.material.textfield.TextInputLayout;
import com.liverussia.cr.R;
import com.liverussia.startMenu.auth.Authorization;
import com.liverussia.startMenu.chooseChar.CharsList;
import com.liverussia.startMenu.servers_list.ServersList;
import com.liverussia.launcher.storage.Storage;

public class GameMenuStart {
    Activity activity;
    TextView login_change_server;

    ServersList serversList;
    CharsList charsList;
    Authorization authorization;

    MaterialButton game_menu_start_play_butt;
    ConstraintLayout loadscreen_main_layout;

    native void connectToServer(String nick, String ip, int port);

    public GameMenuStart(Activity activity) {
        this.activity = activity;

        authorization = new Authorization(this);
        loadscreen_main_layout = activity.findViewById(R.id.loadscreen_main_layout);

        login_change_server = activity.findViewById(R.id.login_change_server);
        login_change_server.setOnClickListener(view -> {
            serversList = new ServersList(this);
        });

        if (Storage.getString("selectedServerName") != null) {
            final String servName = Storage.getString( "selectedServerName");
            login_change_server.setText( String.format("Сервер: %s", servName) );
        }
        else {
            serversList = new ServersList(this);
        }

        game_menu_start_play_butt = activity.findViewById(R.id.game_menu_start_play_butt);
        game_menu_start_play_butt.setOnClickListener(view -> {
            charsList = new CharsList(this);
//
        });

        TextView change_user = activity.findViewById(R.id.change_user);
        change_user.setOnClickListener(view -> {
            change_user.setVisibility(View.GONE);

            Storage.remove("email");
            Storage.remove("password");

            authorization = new Authorization(this);
        });

        ConstraintLayout login_login_layout = activity.findViewById(R.id.login_login_layout);
        login_login_layout.setOnClickListener(view -> {
            if(change_user.getVisibility() == View.GONE)
                change_user.setVisibility(View.VISIBLE);
            else
                change_user.setVisibility(View.GONE);
        });
    }


    public void onCloseAuthorization() {
        authorization = null;

        TextView login_login_text = activity.findViewById(R.id.login_login_text);
        login_login_text.setText( Storage.getString("email") );
    }

    public void onCloseChooseServer() {
        serversList = null;

        activity.runOnUiThread(()-> {
            final String servName = Storage.getString("selectedServerName");
            login_change_server.setText( String.format("Сервер: %s", servName) );
        });
    }

    public void onCloseCharsList() {
        charsList = null;

        if(Storage.getString("nickname") != null)
        {
            // connect
            loadscreen_main_layout.setVisibility(View.GONE);
            connectToServer(
                    Storage.getString("nickname"),
                    Storage.getString("selectedServerIp"),
                    Storage.getInt("selectedServerPort")
            );
        }
    }
}
