package com.liverussia.startMenu.servers_list;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

import lombok.Data;

@Data
@JsonIgnoreProperties(ignoreUnknown = true)
public class ServerData {
    @SerializedName("color")
    @Expose
    protected String color;

    @SerializedName("locked")
    @Expose
    protected int locked;

    @SerializedName("serverID")
    @Expose
    protected String serverID;

    @SerializedName("mult")
    @Expose
    protected String mult;

    @SerializedName("name")
    @Expose
    protected String name;

    @SerializedName("ip")
    @Expose
    protected String ip;

    @SerializedName("port")
    @Expose
    protected int port;

    @SerializedName("online")
    @Expose
    protected int online;

    @SerializedName("maxonline")
    @Expose
    protected int maxonline;

    @SerializedName("rDomain")
    @Expose
    protected String rouletteDomain;
}
