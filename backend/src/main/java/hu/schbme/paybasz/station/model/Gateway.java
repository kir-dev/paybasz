package hu.schbme.paybasz.station.model;

import hu.schbme.paybasz.station.config.AppUtil;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.RequiredArgsConstructor;

import java.beans.Transient;
import java.util.Deque;
import java.util.LinkedList;

@Data
@RequiredArgsConstructor
public class Gateway {

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

    private final String name;
    private final String token;
    private long lastPacket = -1;
    private Deque<CardReading> lastReadings = new LinkedList<>();
    private final boolean physical;

}
