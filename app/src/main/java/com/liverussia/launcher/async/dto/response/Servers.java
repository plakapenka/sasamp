package com.liverussia.launcher.async.dto.response;

import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

public class Servers {

	@SerializedName("color")
	@Expose
	private String color;

	@SerializedName("locked")
	@Expose
	private int locked;

	@SerializedName("serverID")
	@Expose
	private String serverID;

	@SerializedName("mult")
	@Expose
	private String mult;

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

	public Servers(String color, String serverID, String mult, String name, int online, int maxonline, int locked) {
		this.color = color;
		this.locked = locked;
		this.serverID = serverID;
		this.mult = mult;
		this.name = name;
		this.online = online;
		this.maxonline = maxonline;
		this.isSelected = false;
	}

	public String getname() {
		return name;
	}

    public String getMult() {
		 return mult;
	}

    public String getColor() {
		return color;
	}

	public int getLock() {
		return locked;
	}

	public String getServerID() { return serverID; }

	public int getOnline(){
		return online;
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