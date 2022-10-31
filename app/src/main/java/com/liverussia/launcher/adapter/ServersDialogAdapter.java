package com.liverussia.launcher.adapter;

import android.content.Context;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AnimationUtils;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.RecyclerView;

import com.dinuscxj.progressbar.CircleProgressBar;
import com.liverussia.cr.R;
import com.liverussia.cr.core.Config;
import com.liverussia.launcher.activity.ActivitySupportedServerSelection;
import com.liverussia.launcher.activity.dialogs.SelectServerDialog;
import com.liverussia.launcher.model.Servers;

import org.ini4j.Wini;

import java.io.File;
import java.io.IOException;
import java.util.List;

public class ServersDialogAdapter extends RecyclerView.Adapter<ServersAdapter.ServersViewHolder> {
    private Context context;
    private List<Servers> servers;
    private SelectServerDialog selectServerDialog;

    public ServersDialogAdapter(Context context, List<Servers> servers, SelectServerDialog selectServerDialog){
        this.context = context;
        this.servers = servers;
        this.selectServerDialog = selectServerDialog;
    }

    @NonNull
    @Override
    public ServersAdapter.ServersViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View v = LayoutInflater.from(context).inflate(R.layout.item_server, parent, false);
        return new ServersAdapter.ServersViewHolder(v);
    }

    @Override
    public void onBindViewHolder(@NonNull ServersAdapter.ServersViewHolder holder, int position) {

        Servers servers = this.servers.get(position);
        holder.bearPaw.setColorFilter(Color.parseColor("#" + servers.getColor()), PorterDuff.Mode.SRC_ATOP);
        holder.people.setColorFilter(Color.parseColor("#" + servers.getColor()),PorterDuff.Mode.SRC_ATOP);
        //holder.backColor.getBackground().setColorFilter(Color.parseColor("#" + servers.getColor()),PorterDuff.Mode.SRC_ATOP);
        holder.name.setText(servers.getname());
        holder.name.setTextColor(Color.parseColor("#" + servers.getColor()));
       // holder.dopname.setText(servers.getDopname());
        holder.textonline.setText(Integer.toString(servers.getOnline()));
        holder.textmaxonline.setText(new String("/" + Integer.toString(servers.getmaxOnline())));
        holder.progressBar.setProgressStartColor(Color.parseColor("#" + servers.getColor()));
        holder.progressBar.setProgressEndColor(Color.parseColor("#" + servers.getColor()));
        holder.progressBar.setProgress(servers.getOnline());
        holder.progressBar.setMax(servers.getmaxOnline());
        holder.container.setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                view.startAnimation(AnimationUtils.loadAnimation(context, R.anim.button_click));
                selectServerDialog.setSelectedServer(servers);
            }
        });
    }

    @Override
    public int getItemCount() {
        return servers.size();
    }

    public static class ServersViewHolder extends RecyclerView.ViewHolder {

        ConstraintLayout container;
        CircleProgressBar progressBar;
        ImageView x2,bearPaw, people;
        TextView name, dopname, textonline, textmaxonline;
       // LinearLayout backColor;

        public ServersViewHolder(View itemView) {
            super(itemView);

            name = itemView.findViewById(R.id.firstName);
           // dopname = itemView.findViewById(R.id.secondName);
            progressBar = itemView.findViewById(R.id.progressBar);
            bearPaw = itemView.findViewById(R.id.bearPaw);
            x2 = itemView.findViewById(R.id.x2);
            people = itemView.findViewById(R.id.people);
            textonline = itemView.findViewById(R.id.online);
            textmaxonline = itemView.findViewById(R.id.onlineTotal);
           // backColor = itemView.findViewById(R.id.backColor);
            container = itemView.findViewById(R.id.container);
        }
    }

}