package com.liverussia.launcher.fragment;

import android.os.Bundle;

import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.LinearLayoutManager;

import androidx.fragment.app.Fragment;

import android.view.View;
import android.view.animation.AnimationUtils;
import android.view.animation.Animation;
import android.view.LayoutInflater;
import android.view.ViewGroup;

import java.util.List;

import com.liverussia.cr.R;
import com.liverussia.launcher.adapter.NewsAdapter;
import com.liverussia.launcher.model.News;
import com.liverussia.launcher.other.Lists;

import com.liverussia.launcher.adapter.ServersAdapter;
import com.liverussia.launcher.model.Servers;

public class MonitoringFragment extends Fragment {
	
	private RecyclerView recyclerNews;
	private NewsAdapter newsAdapter;
	private List<News> news;

	private RecyclerView recyclerServers;
	private ServersAdapter serversAdapter;
	private List<Servers> servers;
	
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		View inflate = inflater.inflate(R.layout.fragment_monitoring, container, false);
    	Animation animation = AnimationUtils.loadAnimation(getActivity(), R.anim.button_click);
		
	    recyclerNews = inflate.findViewById(R.id.newsRV);

		LinearLayoutManager layoutManager = new LinearLayoutManager(getActivity(), LinearLayoutManager.HORIZONTAL, false);
		recyclerNews.setLayoutManager(layoutManager);
		
		this.news = Lists.NEWS;
		newsAdapter = new NewsAdapter(getContext(), this.news);
		recyclerNews.setAdapter(newsAdapter);
		recyclerNews.setHasFixedSize(true);



		
		recyclerServers = inflate.findViewById(R.id.ourServersRV);

		LinearLayoutManager layoutManagerr = new LinearLayoutManager(getActivity());
		recyclerServers.setLayoutManager(layoutManagerr);
		
		this.servers = Lists.SERVERS;
		serversAdapter = new ServersAdapter(getContext(), this.servers);
		recyclerServers.setAdapter(serversAdapter);

		recyclerServers.setHasFixedSize(true);
		
        return inflate;
    }
}