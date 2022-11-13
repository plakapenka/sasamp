package com.liverussia.launcher.dto.response;

import lombok.Data;
import lombok.EqualsAndHashCode;

@Data
@EqualsAndHashCode
public class FileInfo {
    private String link;
    private String path;
}
