package com.ssmp.startMenu.servers_list;

import static com.ssmp.cr.core.Samp.activity;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.RecyclerView;

import com.ssmp.cr.R;
import com.ssmp.launcher.storage.Storage;

import java.util.List;

public class ServersAdapter extends RecyclerView.Adapter<ServersAdapter.ViewHolder> {
    int selected_item = -1;
    public List<ServerData> items;
    ServersList fromActivity;
    LayoutInflater mInflater;
    //ItemClickListener mClickListener;

    // data is passed into the constructor
    ServersAdapter(Context context, List<ServerData> list, ServersList charsList) {
        this.fromActivity = charsList;
        this.mInflater = LayoutInflater.from(context);
        this.items = list;
    }

    // inflates the cell layout from xml when needed
    @Override
    @NonNull
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = mInflater.inflate(R.layout.select_server_item, parent, false);
        return new ViewHolder(view);
    }

    // binds the data to the TextView in each cell
    @SuppressLint("DefaultLocale")
    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        int color = Color.parseColor(items.get(position).color);
        holder.background.setBackgroundTintList(ColorStateList.valueOf(color));

        holder.name.setText(items.get(position).name);
        holder.onlineText.setText(String.format("%d/1000", items.get(position).online));
        holder.onlineProgress.setProgress(items.get(position).online);
    }

    // total number of cells
    @Override
    public int getItemCount() {
        return items.size();
    }

    // stores and recycles views as they are scrolled off screen
    public class ViewHolder extends RecyclerView.ViewHolder {
        ConstraintLayout background;
        TextView name;
        TextView onlineText;
        ProgressBar onlineProgress;

        ViewHolder(View itemView) {
            super(itemView);
            name = itemView.findViewById(R.id.select_server_item_name);
            background = itemView.findViewById(R.id.select_server_item_background);
            onlineText = itemView.findViewById(R.id.select_server_item_online);
            onlineProgress = itemView.findViewById(R.id.select_server_item_progress);

            itemView.setOnClickListener(view -> {
                int pos = this.getBindingAdapterPosition();

                Storage.setProperty(activity, "selectedServerName", items.get(pos).name);
                Storage.setProperty(activity, "selectedServerIp", items.get(pos).ip);
                Storage.setProperty(activity, "selectedServerPort", items.get(pos).port);
                Storage.setProperty(activity, "selectedServerId", pos);

                fromActivity.toggle(false);
            });
        }
    }
}
