package hu.schbme.paybasz.station.dto;

import com.fasterxml.jackson.databind.annotation.JsonSerialize;
import hu.schbme.paybasz.station.serialize.CsvSerializeable;
import hu.schbme.paybasz.station.serialize.CsvSerializer;

@JsonSerialize(using = CsvSerializer.class)
public enum PaymentStatus implements CsvSerializeable {
    ACCEPTED,
    INTERNAL_ERROR,
    NOT_ENOUGH_CASH,
    VALIDATION_ERROR, // Card or user not found
    CARD_REJECTED,
    UNAUTHORIZED_TERMINAL;

    @Override
    public String csvSerialize() {
        return name();
    }

}
