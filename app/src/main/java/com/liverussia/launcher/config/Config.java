package com.liverussia.launcher.config;

public class Config {

    public static final String FILE_INFO_URL = "https://files.liverussia.online/downloads/files/files.json";
    public static final String URL_RE_CAPTCHA = "https://files.liverussia.online/reCaptcha.html";
    private static final String URL_DONATE = "https://liverussia.online/donate_v2/confirm.php?server=%s&serverName=%s&sum=%s&account=%s&mail=%s&captcha=%s";

    public static final String LIVE_RUSSIA_RESOURCE_SERVER_URL = "https://files.liverussia.online";
    public static final String ROULETTE_SERVER_PROTOCOL = "https://";

    public static final String DISCORD_URI = "https://discord.com/invite/pkT6SEEXKS";
    public static final String VK_URI = "https://vk.com/liverussia.online";
    public static final String YOU_TUBE_URI = "https://www.youtube.com/@LeonidLitvinenko";
    public static final String TELEGRAM_URI = "https://t.me/lr_dev";

    public static final String NATIVE_SETTINGS_FILE_PATH = "/settings.ini";
    public static final String SETTINGS_FILE_PATH = "/gta_sa.set";
    public static final String DOWNLOAD_DIRECTORY_NAME = "/temp_downloads";
    public static final String UPDATED_APK_PATH = "/liverussia.apk";

    public static String createBillingUri(String serverId, String serverName, String sum, String nickname, String mail, String captcha) {
        return String.format(
                URL_DONATE,
                serverId,
                serverName,
                sum,
                nickname,
                mail,
                captcha
        );
    }
}