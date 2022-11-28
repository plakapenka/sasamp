package com.liverussia.launcher.dto.response;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

import lombok.Data;

@Data
@JsonIgnoreProperties(ignoreUnknown = true)
public class PrizeInfoResponseDto {
    private String additionalInfo;
    private String base64Image;
}
