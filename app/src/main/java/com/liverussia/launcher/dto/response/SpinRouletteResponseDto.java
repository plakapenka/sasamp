package com.liverussia.launcher.dto.response;

import java.util.List;

import lombok.Data;

@Data
public class SpinRouletteResponseDto {
    private PrizeInfoResponseDto prizeInfo;
    private List<String> base64Images;
    private String countElementsInOneSpin;
    private String spinDurationInMillis;
    private String balance;
}
