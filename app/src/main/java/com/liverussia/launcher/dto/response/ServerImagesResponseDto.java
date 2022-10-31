package com.liverussia.launcher.dto.response;

import com.google.gson.annotations.Expose;

import java.util.List;

import lombok.Data;

@Data
public class ServerImagesResponseDto {
    @Expose
    private List<String> urls;
}
