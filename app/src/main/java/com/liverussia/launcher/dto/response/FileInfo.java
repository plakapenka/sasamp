package com.liverussia.launcher.dto.response;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

import lombok.Data;
import lombok.EqualsAndHashCode;

@Data
@EqualsAndHashCode
@JsonIgnoreProperties(ignoreUnknown = true)
public class FileInfo {
    private String link;
    private String path;
    private String hash;
    private long size;
}
