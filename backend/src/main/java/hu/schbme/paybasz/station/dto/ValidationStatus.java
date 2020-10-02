package hu.schbme.paybasz.station.dto;

import com.fasterxml.jackson.databind.annotation.JsonSerialize;
import hu.schbme.paybasz.station.serialize.CsvSerializeable;
import hu.schbme.paybasz.station.serialize.CsvSerializer;

@JsonSerialize(using = CsvSerializer.class)
public enum ValidationStatus implements CsvSerializeable {
    OK,
    INVALID;

    @Override
    public String csvSerialize() {
        return name();
    }

}
