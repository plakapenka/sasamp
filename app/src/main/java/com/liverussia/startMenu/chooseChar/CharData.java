package com.liverussia.startMenu.chooseChar;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

import lombok.Data;

@Data
@JsonIgnoreProperties(ignoreUnknown = true)
public class CharData {
    protected String nickname;
    protected int money;
    protected int level;
    protected int skin;
}
