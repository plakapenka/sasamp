package com.liverussia.launcher.domain;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.Getter;
import lombok.NoArgsConstructor;

@Getter
public class RouletteItem {
    //set to String, if you want to add image url from internet
    private Integer resourceImage;
    private String base64Image;

    public RouletteItem(int resourceImage) {
        this.resourceImage = resourceImage;
    }

    public RouletteItem(String base64Image) {
        this.base64Image = base64Image;
    }
}
