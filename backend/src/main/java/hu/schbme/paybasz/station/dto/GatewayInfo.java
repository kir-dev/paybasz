package hu.schbme.paybasz.station.dto;

import hu.schbme.paybasz.station.config.AppUtil;
import hu.schbme.paybasz.station.model.Gateway.CardReading;
import lombok.AllArgsConstructor;
import lombok.Data;

import javax.persistence.Transient;
import java.util.Deque;

@Data
@AllArgsConstructor
public class GatewayInfo {

    private static final String JUST_NOW = "épp most";
    private static final String NO_DATA = "nincs adat";
    private static final String MINUTE = "p";
    private static final String SECOND = "mp";
    private static final String HOURS = "ó";

    private String name;
    private long lastPacket;
    private Deque<CardReading> lastReadings;
    private long txCount;
    private long allTraffic;
    private boolean physical;

    @Transient
    public String getLastPacketFormatted() {
        if (lastPacket < 0)
            return NO_DATA;
        long formatted = System.currentTimeMillis() - lastPacket;
        formatted /= 1000;
        if (formatted == 0)
            return JUST_NOW;
        if (formatted < 60)
            return formatted + SECOND;
        formatted /= 60;
        if (formatted < 60)
            return formatted + MINUTE;
        formatted /= 60;
        return formatted + HOURS;
    }

    @Transient
    public String getAllTrafficFormatted() {
        return AppUtil.formatNumber(allTraffic);
    }


}
