package com.liverussia.launcher.dto.response;

import lombok.Data;

@Data
public class LatestVersionInfoDto {
    private String version;

    private String link;

    private String path;

    private long size;
}
