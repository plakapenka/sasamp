package com.liverussia.launcher.config;

public class Config {
    //todo   тоооо
    public static final String FILE_INFO_URL = "https://files.liverussia.online/downloads/files/files.json";

    public static final String DISCORD_URI = "https://discord.com/";
    public static final String VK_URI = "https://vk.com/";
    public static final String YOU_TUBE_URI = "https://www.youtube.com/";

//    public static final String LAUNCHER_SERVER_URI = "http://192.168.0.100:9400";
    public static final String LIVE_RUSSIA_RESOURCE_SERVER_URI = "https://files.liverussia.online";
    public static final String LAUNCHER_SERVER_URI = "http://94.250.248.153:9400";

    public static final String LAUNCHER_SERVER_LOGIN_URL = "/api/v1/auth/android/login";
    public static final String LAUNCHER_SERVER_REFRESH_TOKEN_URL = "/api/v1/auth/android/refresh";
    public static final String LAUNCHER_SERVER_UPDATE_USER_INFO_URL = "/api/v1/android/user/info";
    public static final String LAUNCHER_SERVER_GET_POSSIBLE_PRIZES_URL = "/api/v1/android/setting/prizes/info";
    public static final String LAUNCHER_SERVER_GET_DONATE_SERVICES_URL = "/api/v1/android/setting/donate/services";
    public static final String LAUNCHER_SERVER_GET_LOADER_SLIDER_INFO_SERVICES_URL = "/api/v1/android/setting/loader/slider/info";
    public static final String LAUNCHER_SERVER_SPIN_ROULETTE_URL = "/api/v1/android/user/roulette/spin";

    public static final String NATIVE_SETTINGS_FILE_PATH = "/SAMP/settings.ini";
    public static final String SETTINGS_FILE_PATH = "/gta_sa.set";
    public static final String DOWNLOAD_DIRECTORY_NAME = "/temp_downloads";
    public static final String UPDATED_APK_PATH = "/liverussia.apk";

    public static final String URL_RE_CAPTCHA = "https://files.liverussia.online/reCaptcha.html";

    public static final String URL_FILES = "https://vl4sov.com/Download/game.zip";
    public static final String URL_FILES_UPDATE = "https://vl4sov.com/Download/upd.zip";
    public static final String URL_CLIENT = "https://vl4sov.com/Download/launcher.apk";
    private static final String URL_DONATE = "https://liverussia.online/donate/confirm.php?server=%s&serverName=%s&sum=%s&account=%s&mail=%s&g-recaptcha-response=%s";

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