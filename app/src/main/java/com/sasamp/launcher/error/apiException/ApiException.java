package com.sasamp.launcher.error.apiException;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
@EqualsAndHashCode(callSuper = true)
public class ApiException extends RuntimeException {

    private String message;

    private ErrorContainer error;

    private String originalFaultString;

    private String originalFaultCode;

    public ApiException(ErrorContainer error) {
        this.error = error;
        this.message = error.getMessage();
    }

    public ApiException(ErrorContainer error, String message) {
        this.error = error;
        this.message = message;
    }

    public ApiException(String message, ErrorContainer error) {
        this.error = error;
        this.message = message;
    }

    public ApiException(ErrorContainer error, String message, String originalFaultString) {
        this.error = error;
        this.message = message;
        this.originalFaultString = originalFaultString;
    }

    public ApiException(ErrorContainer error, String message, String originalFaultString, String originalFaultCode) {
        this.error = error;
        this.message = message;
        this.originalFaultString = originalFaultString;
        this.originalFaultCode = originalFaultCode;
    }

    public ApiException(ApiException exception) {
        this.error = exception.getError();
        this.message = exception.getMessage();
        this.originalFaultString = exception.getOriginalFaultString();
        this.originalFaultCode = exception.getOriginalFaultCode();
    }
}
