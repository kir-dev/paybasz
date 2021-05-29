package hu.schbme.paybasz.station.config;

import java.text.SimpleDateFormat;

public final class AppUtil {

    public static final SimpleDateFormat DATE_TIME_FORMATTER = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
    public static final SimpleDateFormat DATE_TIME_FILE_FORMATTER = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss");
    public static final SimpleDateFormat DATE_ONLY_FORMATTER = new SimpleDateFormat("yyyy-MM-dd");
    public static final SimpleDateFormat TIME_ONLY_FORMATTER = new SimpleDateFormat("HH:mm:ss");

    public static String formatNumber(long number) {
        var nums = String.valueOf(number);
        var result = new StringBuilder();
        for (int i = nums.length() - 1; i >= 0; i--) {
            result.insert(0, ((nums.length() - i) % 3 == 0 ? " " : "") + nums.charAt(i));
        }
        return result.toString();
    }

}
