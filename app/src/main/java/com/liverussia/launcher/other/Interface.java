package com.liverussia.launcher.other;

import com.liverussia.launcher.model.News;
import com.liverussia.launcher.model.Servers;

import java.util.List;

import retrofit2.Call;
import retrofit2.http.GET;

public interface Interface {

    @GET("https://vl4sov.com/liverussia.online/servers.json")
    //@GET("https://api.open.mp/servers/46.174.49.47:7825")
    Call<List<Servers>> getServers();

    @GET("https://vl4sov.com/liverussia.online/history.json")
    Call<List<News>> getNews();

}
