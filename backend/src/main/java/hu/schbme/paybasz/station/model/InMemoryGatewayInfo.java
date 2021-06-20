package hu.schbme.paybasz.station.model;

import hu.schbme.paybasz.station.config.AppUtil;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.Getter;
import lombok.Setter;

import java.beans.Transient;
import java.util.Deque;
import java.util.LinkedList;

@Getter
@Setter
public final class InMemoryGatewayInfo {

    @Data
    @AllArgsConstructor
    public static class CardReading {
        private String card;
        private long time;

        @Transient
        public String getTimeFormatted() {
            return AppUtil.DATE_TIME_FORMATTER.format(time);
        }
    }

    private long lastPacket = -1;
    private final Deque<CardReading> lastReadings = new LinkedList<>();

}
