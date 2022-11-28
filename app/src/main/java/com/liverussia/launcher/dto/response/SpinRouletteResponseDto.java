package com.liverussia.launcher.dto.response;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

import java.util.List;

import lombok.Data;

@Data
@JsonIgnoreProperties(ignoreUnknown = true)
public class SpinRouletteResponseDto {
    private PrizeInfoResponseDto prizeInfo;
    private List<String> base64Images;
    private String countElementsInOneSpin;
    private String spinDurationInMillis;
    private String balance;
}
