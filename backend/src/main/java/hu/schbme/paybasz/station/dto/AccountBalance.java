package hu.schbme.paybasz.station.dto;

import com.fasterxml.jackson.databind.annotation.JsonSerialize;
import hu.schbme.paybasz.station.serialize.CsvSerializeable;
import hu.schbme.paybasz.station.serialize.CsvSerializer;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
@JsonSerialize(using = CsvSerializer.class)
public class AccountBalance implements CsvSerializeable {

    private int balance;
    private boolean loadAllowed;
    private boolean allowed;

    @Override
    public String csvSerialize() {
        return balance + ";" + (loadAllowed ? "1" : "0") + ";" + (allowed ? "1" : "0");
    }

}
