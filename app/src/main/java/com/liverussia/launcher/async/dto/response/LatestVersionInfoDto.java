package com.liverussia.launcher.async.dto.response;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

import lombok.Data;

@Data
@JsonIgnoreProperties(ignoreUnknown = true)
public class LatestVersionInfoDto {
    private String version;

    private String link;

    private String path;

    private long size;
}
