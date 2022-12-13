package com.liverussia.launcher.ui.adapters;

import android.graphics.PorterDuff;
import android.graphics.Color;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.animation.AnimationUtils;
import android.content.Context;
import android.view.View;
import android.view.ViewGroup;
import android.widget.*;

import androidx.recyclerview.widget.RecyclerView;
import androidx.annotation.NonNull;

import com.liverussia.cr.R;
import com.liverussia.launcher.domain.enums.NativeStorageElements;
import com.liverussia.launcher.domain.messages.InfoMessage;
import com.liverussia.launcher.async.dto.response.Servers;

import java.io.File;
import java.io.IOException;
import java.util.List;

import com.dinuscxj.progressbar.CircleProgressBar;
import com.liverussia.launcher.service.ActivityService;
import com.liverussia.launcher.service.impl.ActivityServiceImpl;
import com.liverussia.launcher.storage.NativeStorage;

import org.apache.commons.lang3.StringUtils;
import org.ini4j.Wini;

public class ServersAdapter extends RecyclerView.Adapter<ServersAdapter.ServersViewHolder> {
	private Context context;
	private List<Servers> servers;
	private int selectedItem;
//	private Boolean isselect[10];
	private final ActivityService activityService;

	{
		activityService = new ActivityServiceImpl();
	}

	public ServersAdapter(Context context, List<Servers> servers){
		 this.context = context;
		 this.servers = servers;
		 selectedItem = Integer.parseInt(NativeStorage.getClientProperty(NativeStorageElements.SERVER, context));
	}

	@NonNull
	@Override
    public ServersViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View v = LayoutInflater.from(context).inflate(R.layout.item_server, parent, false);
		return new ServersViewHolder(v);
    }

    @Override
    public void onBindViewHolder(ServersViewHolder holder, int position) {
		Servers servers = this.servers.get(position);
		int MainColor = Color.parseColor(servers.getColor());
		int LightColor = Color.parseColor(servers.getColorl());

		holder.bearPaw.setColorFilter(LightColor, PorterDuff.Mode.SRC_ATOP);
		holder.people.setColorFilter(LightColor, PorterDuff.Mode.SRC_ATOP);
		holder.backColor.getBackground().setColorFilter(MainColor, PorterDuff.Mode.SRC_ATOP);
		holder.name.setText(servers.getname());
		holder.name.setTextColor(LightColor);
		holder.dopname.setText(servers.getDopname());
		holder.textonline.setText(Integer.toString(servers.getOnline()));
		holder.textmaxonline.setText(new String("/" + Integer.toString(servers.getmaxOnline())));
	    holder.progressBar.setProgressStartColor(MainColor);
		holder.progressBar.setProgressEndColor(LightColor);
		holder.progressBar.setProgress(servers.getOnline());
		holder.progressBar.setMax(servers.getmaxOnline());

		if(selectedItem == position){

			holder.container.setScaleX(1.05f);
			holder.container.setScaleY(1.05f);
			holder.backColor.setAlpha(0.55f);
		}else{
			holder.container.setScaleX(1.0f);
			holder.container.setScaleY(1.0f);
			holder.backColor.setAlpha(0.40f);
		}

		holder.container.setOnClickListener(view -> {
			selectedItem = position;
			this.notifyDataSetChanged();

			//view.startAnimation(AnimationUtils.loadAnimation(context, R.anim.button_click));
			NativeStorage.addClientProperty(NativeStorageElements.SERVER, servers.getServerID(), context);

			//String selectedServer = NativeStorage.getClientProperty(NativeStorageElements.SERVER, context);

			//if (StringUtils.isNotBlank(selectedServer)) {
			activityService.showMessage(InfoMessage.SERVER_SELECTED.getText(), context);
			//}
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
		TextView name, dopname, textonline, textmaxonline;
		LinearLayout backColor;

        public ServersViewHolder(View itemView) {
            super(itemView);

		    name = itemView.findViewById(R.id.firstName);
			dopname = itemView.findViewById(R.id.secondName);
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