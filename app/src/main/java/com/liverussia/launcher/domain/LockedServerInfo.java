package com.liverussia.launcher.domain;

import com.liverussia.launcher.async.dto.response.Servers;

import lombok.Data;

@Data
public class LockedServerInfo {
    private Servers servers;

    private int position;

    private String password;
}
