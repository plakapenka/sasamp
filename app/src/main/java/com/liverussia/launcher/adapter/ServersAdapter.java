package com.liverussia.launcher.adapter;

import android.content.Context;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AnimationUtils;
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
import com.liverussia.launcher.model.Servers;

import org.ini4j.Wini;

import java.io.File;
import java.io.IOException;
import java.util.List;

public class ServersAdapter extends RecyclerView.Adapter<ServersAdapter.ServersViewHolder> {
	private Context context;
	private List<Servers> servers;

	public ServersAdapter(Context context, List<Servers> servers){
		 this.context = context;
		 this.servers = servers;
	}

	@NonNull
	@Override
    public ServersViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View v = LayoutInflater.from(context).inflate(R.layout.item_server, parent, false);
		return new ServersViewHolder(v);
    }

    @Override
    public void onBindViewHolder(@NonNull ServersViewHolder holder, int position) {

        Servers servers = this.servers.get(position);
		int color = Color.parseColor("#" + servers.getColor());
		ColorStateList colorlist = ColorStateList.valueOf(color);

		//holder.bearPaw.setColorFilter(color);
		//holder.people.setColorFilter(color);

		holder.people.setImageTintList(colorlist);
		holder.bearPaw.setImageTintList(colorlist);

		holder.back_color.setBackgroundTintList(colorlist);
		holder.back_color.getBackground().setAlpha(100);

		//holder.bearPaw.setBackgroundTintList(colorlist);

		//holder.back_color.getBackground().setColorFilter(color,PorterDuff.Mode.DST);
		holder.name.setText(servers.getname());
		holder.name.setTextColor(color);
		//holder.dopname.setText(servers.getDopname());
		holder.textonline.setText(Integer.toString(servers.getOnline()));
		holder.textmaxonline.setText(String.format("/%d", servers.getmaxOnline()));
	    holder.progressBar.setProgressStartColor(color);
		holder.progressBar.setProgressEndColor(color);
		holder.progressBar.setProgress(servers.getOnline());
		holder.progressBar.setMax(servers.getmaxOnline());
		holder.container.setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                view.startAnimation(AnimationUtils.loadAnimation(context, R.anim.button_click));
                Toast.makeText(context, "Для начала игры нажмите жёлтую кнопку", 0).show();
				try {
					File f = new File(Config.GAME_PATH + "SAMP/settings.ini");
					if (!f.exists()) { f.createNewFile(); f.mkdirs(); }
					Wini w = new Wini(new File(Config.GAME_PATH + "SAMP/settings.ini"));
					w.put("client", "server", servers.getServerID());
					w.store();
				} catch (IOException e) {
					e.printStackTrace();
				}
            }
        });
    }

    @Override
    public int getItemCount() {
        return servers.size();
    }

    public static class ServersViewHolder extends RecyclerView.ViewHolder {

		ConstraintLayout container;
		View back_color;
		CircleProgressBar progressBar;
		TextView x2;
		ImageView bearPaw, people;
		TextView name, dopname, textonline, textmaxonline;
		LinearLayout backColor;

        public ServersViewHolder(View itemView) {
            super(itemView);

		    name = itemView.findViewById(R.id.firstName);
			//dopname = itemView.findViewById(R.id.secondName);
			progressBar = itemView.findViewById(R.id.progressBar);
			bearPaw = itemView.findViewById(R.id.bearPaw);
			x2 = itemView.findViewById(R.id.x2);
			people = itemView.findViewById(R.id.people);
			textonline = itemView.findViewById(R.id.online);
			textmaxonline = itemView.findViewById(R.id.onlineTotal);
			back_color = itemView.findViewById(R.id.back_color);
			container = itemView.findViewById(R.id.container);
        }
    }

}