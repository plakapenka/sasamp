package com.ssmp.launcher.utils;

import android.util.Base64;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.ssmp.launcher.domain.Token;

import org.threeten.bp.LocalDateTime;

public class TokenUtils {

    private static final int REQUEST_DURATION_IN_SECONDS = 20;

    public static Token decodeToken(String encodeToken) {
        String[] chunks = encodeToken.split("\\.");

        if (chunks.length < 2) {
            return null;
        }

        String payload = new String(Base64.decode(chunks[1], Base64.DEFAULT));
        ObjectMapper mapper = new ObjectMapper();
        Token token;

        try {
            token = mapper.readValue(payload, Token.class);
        } catch (Exception exception) {
            exception.printStackTrace();
            return null;
        }

        return token;
    }

    public static LocalDateTime getLocalExpirationTime(Token token, LocalDateTime tokenReceiveTime) {
        if (token.getExpiration() == null || token.getIssuedAt() == null) {
            return tokenReceiveTime;
        }

        long expirationSeconds = Long.parseLong(token.getExpiration());
        long issuedAtSeconds = Long.parseLong(token.getIssuedAt());

        long lifetimeSeconds = expirationSeconds - issuedAtSeconds;

        return tokenReceiveTime.plusSeconds(lifetimeSeconds)
                .minusSeconds(REQUEST_DURATION_IN_SECONDS);
    }


}
