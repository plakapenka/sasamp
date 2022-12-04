package com.liverussia.launcher.async.dto.response;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

import java.util.List;

import lombok.Data;

@Data
@JsonIgnoreProperties(ignoreUnknown = true)
public class MonitoringData {
    private List<News> news;
    private List<Servers> servers;
}
