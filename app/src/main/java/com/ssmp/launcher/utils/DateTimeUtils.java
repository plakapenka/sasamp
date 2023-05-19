package com.ssmp.launcher.utils;

import org.threeten.bp.LocalDateTime;
import org.threeten.bp.ZoneId;

public class DateTimeUtils {

    private static final String UTC_ZONE_ID = "UTC";

    public static LocalDateTime getUTCNow() {
        return LocalDateTime.now(ZoneId.of(UTC_ZONE_ID));
    }
}
