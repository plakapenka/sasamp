package com.liverussia.launcher.dto.response;

import java.util.List;

import lombok.Data;

@Data
public class GameFileInfoDto {
    private Long allFilesSize;
    private List<FileInfo> files;
}
