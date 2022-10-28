package com.liverussia.launcher.dto.response;

import java.util.List;

import lombok.Data;

@Data
public class ServerImagesResponseDto {
    private List<String> base64Images;
}
