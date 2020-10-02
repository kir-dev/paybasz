package hu.schbme.paybasz.station.dto;

import lombok.AllArgsConstructor;
import lombok.Getter;

@Getter
@AllArgsConstructor
public enum LogSeverity {
    FAILURE("danger"),
    ACTION("secondary"),
    SUCCESS("success"),
    SERVER_INFO("dark"),
    SERVER_WARNING("warning"),
    ERROR("danger"),
    NOTE("info");

    private final String color;
}
