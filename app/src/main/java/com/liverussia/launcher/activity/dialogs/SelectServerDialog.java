package com.liverussia.launcher.activity.dialogs;

import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageButton;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.fragment.app.DialogFragment;
import androidx.fragment.app.FragmentManager;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.liverussia.cr.R;
import com.liverussia.launcher.activity.ActivitySupportedServerSelection;
import com.liverussia.launcher.adapter.ServersAdapter;
import com.liverussia.launcher.adapter.ServersDialogAdapter;
import com.liverussia.launcher.enums.ServerInfo;
import com.liverussia.launcher.model.Servers;
import com.liverussia.launcher.other.Lists;

import java.util.ArrayList;

public class SelectServerDialog extends DialogFragment implements View.OnClickListener {

    public static final String TAG = "select_server_dialog";

    private ActivitySupportedServerSelection activity;

    private TextView btnGoBack;

    private RecyclerView recyclerServers;
    private ServersDialogAdapter serversAdapter;

    public static SelectServerDialog display(FragmentManager fragmentManager, ActivitySupportedServerSelection activity) {
        SelectServerDialog dialog = new SelectServerDialog(activity);
        dialog.show(fragmentManager, TAG);
        return dialog;
    }

    public SelectServerDialog(ActivitySupportedServerSelection activity) {
        super();
        this.activity = activity;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setStyle(DialogFragment.STYLE_NORMAL, R.style.AppTheme_FullScreenDialog);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        super.onCreateView(inflater, container, savedInstanceState);
        View view = inflater.inflate(R.layout.select_server_dialog_activity, container, false);
        getDialog().getWindow().setBackgroundDrawable(getResources().getDrawable(R.drawable.auth_choose_serv_bg));

        btnGoBack = view.findViewById(R.id.btnGoBack);
        btnGoBack.setOnClickListener(this);

        recyclerServers = view.findViewById(R.id.ourServersRV);
        recyclerServers.setHasFixedSize(true);
        LinearLayoutManager layoutManagerr = new LinearLayoutManager(getActivity());
        recyclerServers.setLayoutManager(layoutManagerr);

        serversAdapter = new ServersDialogAdapter(getContext(), Lists.SERVERS, this);
        recyclerServers.setAdapter(serversAdapter);

        return view;
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btnGoBack:
                dismiss();
                break;
            default:
                break;
        }
    }

    public void setSelectedServer(Servers server) {
        activity.setSelectedServer(server);
        dismiss();
    }
}
