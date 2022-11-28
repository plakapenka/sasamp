package com.liverussia.launcher.dto.response;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.google.gson.annotations.Expose;

import java.util.List;

import lombok.Data;

@Data
@JsonIgnoreProperties(ignoreUnknown = true)
public class ServerImagesResponseDto {
    @Expose
    private List<String> urls;
}
