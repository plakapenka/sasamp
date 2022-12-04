package com.liverussia.launcher.fragment;

import android.os.Bundle;

import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.LinearLayoutManager;

import androidx.fragment.app.Fragment;

import android.view.View;
import android.view.LayoutInflater;
import android.view.ViewGroup;

import java.util.List;

import com.liverussia.cr.R;
import com.liverussia.launcher.adapter.NewsAdapter;
import com.liverussia.launcher.dto.response.News;
import com.liverussia.launcher.other.Lists;

import com.liverussia.launcher.adapter.ServersAdapter;
import com.liverussia.launcher.dto.response.Servers;

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
		
	    recyclerNews = inflate.findViewById(R.id.newsRV);
		recyclerNews.setHasFixedSize(true);
		
		recyclerServers = inflate.findViewById(R.id.ourServersRV);
		recyclerServers.setHasFixedSize(true);

		attachNews();
		attachServers();
		
        return inflate;
    }

    public void attachNews() {
		LinearLayoutManager layoutManager = new LinearLayoutManager(getActivity(), LinearLayoutManager.HORIZONTAL, false);
		recyclerNews.setLayoutManager(layoutManager);

		this.news = Lists.NEWS;
		newsAdapter = new NewsAdapter(getContext(), this.news);
		recyclerNews.setAdapter(newsAdapter);
	}

	public void attachServers() {
		LinearLayoutManager layoutManagerr = new LinearLayoutManager(getActivity());
		recyclerServers.setLayoutManager(layoutManagerr);

		this.servers = Lists.SERVERS;
		serversAdapter = new ServersAdapter(getContext(), this.servers);
		recyclerServers.setAdapter(serversAdapter);
	}


}