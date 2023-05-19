package com.ssmp.launcher.error.apiException;

import lombok.AllArgsConstructor;
import lombok.Data;

@Data
@AllArgsConstructor
public class ApiError {

    private String message;

    private int errorCode;
}
