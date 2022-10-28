package com.liverussia.launcher.service;

import com.liverussia.launcher.dto.response.ServerImagesResponseDto;
import com.liverussia.launcher.dto.response.SpinRouletteResponseDto;

public interface RouletteService {
    ServerImagesResponseDto getPossiblePrizes();

    SpinRouletteResponseDto spinRoulette();
}
