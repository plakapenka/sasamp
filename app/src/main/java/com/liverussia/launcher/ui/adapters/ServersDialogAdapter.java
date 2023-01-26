package com.liverussia.launcher.ui.adapters;

import static com.liverussia.launcher.config.Config.ROULETTE_SERVER_PROTOCOL;

import android.content.Context;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AnimationUtils;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.dinuscxj.progressbar.CircleProgressBar;
import com.liverussia.cr.R;
import com.liverussia.launcher.domain.enums.StorageElements;
import com.liverussia.launcher.storage.Storage;
import com.liverussia.launcher.ui.dialogs.SelectServerDialog;
import com.liverussia.launcher.async.dto.response.Servers;

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
        int MainColor = Color.parseColor(servers.getColor());
//        int LightColor = Color.parseColor(servers.getColorl());

        holder.bearPaw.setColorFilter(MainColor, PorterDuff.Mode.SRC_ATOP);
        holder.people.setColorFilter(MainColor, PorterDuff.Mode.SRC_ATOP);
        holder.backColor.getBackground().setColorFilter(MainColor, PorterDuff.Mode.SRC_ATOP);
        holder.name.setText(servers.getname());
        holder.name.setTextColor(MainColor);
        holder.mult.setText(servers.getMult());
        holder.textonline.setText(Integer.toString(servers.getOnline()));
        holder.textmaxonline.setText("/1000");
        holder.progressBar.setProgressStartColor(MainColor);
        holder.progressBar.setProgressEndColor(MainColor);
        holder.progressBar.setProgress(servers.getOnline());
        holder.container.setOnClickListener(view -> {
            view.startAnimation(AnimationUtils.loadAnimation(context, R.anim.button_click));
            selectServerDialog.setSelectedServer(servers);

            String rouletteUrl = ROULETTE_SERVER_PROTOCOL
                    .concat(servers.getRouletteDomain());

            Storage.addProperty(StorageElements.ROULETTE_SERVER_HOST.getValue(), rouletteUrl, context);
        });
    }

    @Override
    public int getItemCount() {
        return servers.size();
    }

    public static class ServersViewHolder extends RecyclerView.ViewHolder {

        FrameLayout container;
        CircleProgressBar progressBar;
        ImageView x2,bearPaw, people;
        TextView name, mult, textonline, textmaxonline;
        LinearLayout backColor;

        public ServersViewHolder(View itemView) {
            super(itemView);

            name = itemView.findViewById(R.id.firstName);
            mult = itemView.findViewById(R.id.secondName);
            progressBar = itemView.findViewById(R.id.progressBar);
            bearPaw = itemView.findViewById(R.id.bearPaw);
            x2 = itemView.findViewById(R.id.x2);
            people = itemView.findViewById(R.id.people);
            textonline = itemView.findViewById(R.id.online);
            textmaxonline = itemView.findViewById(R.id.onlineTotal);
            backColor = itemView.findViewById(R.id.backColor);
            container = itemView.findViewById(R.id.container);
        }
    }

}