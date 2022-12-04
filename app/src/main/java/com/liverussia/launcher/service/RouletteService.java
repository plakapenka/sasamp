package com.liverussia.launcher.service;

import com.liverussia.launcher.async.dto.response.ServerImagesResponseDto;
import com.liverussia.launcher.async.dto.response.SpinRouletteResponseDto;

public interface RouletteService {
    ServerImagesResponseDto getPossiblePrizes();

    SpinRouletteResponseDto spinRoulette();
}
