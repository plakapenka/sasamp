package com.liverussia.launcher.activity.dialogs;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
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
import com.liverussia.launcher.enums.ServerInfo;
import com.liverussia.launcher.model.Servers;
import com.liverussia.launcher.other.Lists;

import java.util.ArrayList;

public class SelectServerDialog extends DialogFragment implements View.OnClickListener {

    public static final String TAG = "select_server_dialog";

    private ActivitySupportedServerSelection activity;

    private TextView btnGoBack;
    private ConstraintLayout moscowItem;
    private ConstraintLayout petersburgItem;
    private ConstraintLayout novosibirskItem;
    private ConstraintLayout samaraItem;
    private ConstraintLayout sochiItem;

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

        btnGoBack = view.findViewById(R.id.btnGoBack);
        btnGoBack.setOnClickListener(this);

        moscowItem = view.findViewById(R.id.moscow);
        moscowItem.setOnClickListener(this);
        petersburgItem = view.findViewById(R.id.peterburg);
        petersburgItem.setOnClickListener(this);
        novosibirskItem = view.findViewById(R.id.novosibirsk);
        novosibirskItem.setOnClickListener(this);
        samaraItem = view.findViewById(R.id.samara);
        samaraItem.setOnClickListener(this);
        sochiItem = view.findViewById(R.id.sochi);
        sochiItem.setOnClickListener(this);

        return view;
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.moscow:
                activity.setSelectedServer(ServerInfo.MOSCOW);
                dismiss();
                break;
            case R.id.peterburg:
                activity.setSelectedServer(ServerInfo.PETERSBURG);
                dismiss();
                break;
            case R.id.novosibirsk:
                activity.setSelectedServer(ServerInfo.NOVOSIBIRSK);
                dismiss();
                break;
            case R.id.samara:
                activity.setSelectedServer(ServerInfo.SAMARA);
                dismiss();
                break;
            case R.id.sochi:
                activity.setSelectedServer(ServerInfo.SOCHI);
                dismiss();
                break;
            case R.id.btnGoBack:
                dismiss();
                break;
            default:
                break;
        }
    }
}
