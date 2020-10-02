package hu.schbme.paybasz.station.config;

import java.text.SimpleDateFormat;

public class AppUtil {

    public static SimpleDateFormat DATE_TIME_FORMATTER = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
    public static SimpleDateFormat DATE_TIME_FILE_FORMATTER = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss");
    public static SimpleDateFormat DATE_ONLY_FORMATTER = new SimpleDateFormat("yyyy-MM-dd");
    public static SimpleDateFormat TIME_ONLY_FORMATTER = new SimpleDateFormat("HH:mm:ss");

    public static String formatNumber(long number) {
        String nums = String.valueOf(number);
        StringBuilder result = new StringBuilder();
        for (int i = nums.length() - 1; i >= 0; i--) {
            result.insert(0, ((nums.length() - i) % 3 == 0 ? " " : "") + nums.charAt(i));
        }
        return result.toString();
    }

}
