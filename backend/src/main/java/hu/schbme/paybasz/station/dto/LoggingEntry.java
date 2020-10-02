package hu.schbme.paybasz.station.dto;

import lombok.AllArgsConstructor;
import lombok.Getter;

import static hu.schbme.paybasz.station.config.AppUtil.DATE_TIME_FORMATTER;

@Getter
@AllArgsConstructor
public class LoggingEntry {

    private long timestamp;
    private LogSeverity severity;
    private String message;

    public String getFormattedDate() {
        return DATE_TIME_FORMATTER.format(timestamp).replace(" ", "&nbsp;");
    }

    public String getFormattedMessage() {
        return message
                .replaceAll("<badge>", "<span class=\"badge badge-" + severity.getColor() + "\">")
                .replaceAll("</badge>", "</span>")
                .replaceAll("<color>", "<span class=\"text-" + severity.getColor() + "\">")
                .replaceAll("</color>", "</span>");

    }

    public String getColor() {
        return severity.getColor();
    }

    public String getMarkdownMessage() {
        return message.replaceAll("</?badge>", "`").replaceAll("</?color>", "*");
    }

}
