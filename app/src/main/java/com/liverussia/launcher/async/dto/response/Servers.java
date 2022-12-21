package com.liverussia.launcher.async.dto.response;

import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

public class Servers {

	@SerializedName("color")
	@Expose
	private String color;

	@SerializedName("colorl")
	@Expose
	private String colorl;

	@SerializedName("serverID")
	@Expose
	private String serverID;

	@SerializedName("dopname")
	@Expose
	private String dopname;

	@SerializedName("name")
	@Expose
	private String name;

	@SerializedName("online")
	@Expose
	private int online;

	@SerializedName("maxonline")
	@Expose
	private int maxonline;

	@SerializedName("rIP")
	@Expose
	private String rouletteIp;

	@SerializedName("rPort")
	@Expose
	private String roulettePort;

	public boolean isSelected;

	public Servers(String color, String serverID, String dopname, String name, int online, int maxonline, String colorl) {
		this.color = color;
		this.colorl = colorl;
		this.serverID = serverID;
		this.dopname = dopname;
		this.name = name;
		this.online = online;
		this.maxonline = maxonline;
		this.isSelected = false;
	}

	public String getname() {
		return name;
	}

    public String getDopname() {
		 return dopname;
	}

    public String getColor() {
		return color;
	}

	public String getColorl() {
		return colorl;
	}

	public String getServerID() { return serverID; }

	public int getOnline(){
		return online;
	}

	public int getmaxOnline(){
		return maxonline;
	}

	public String getRouletteIp() {
		return rouletteIp;
	}

	public void setRouletteIp(String rouletteIp) {
		this.rouletteIp = rouletteIp;
	}

	public String getRoulettePort() {
		return roulettePort;
	}

	public void setRoulettePort(String roulettePort) {
		this.roulettePort = roulettePort;
	}
}