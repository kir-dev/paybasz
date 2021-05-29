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
public class ItemQueryResult implements CsvSerializeable {

    private boolean valid;
    private String name;
    private int price;

    @Override
    public String csvSerialize() {
        return (valid ? "1" : "0") + ";" + name.replace(';', ':') + ";" + price;
    }

}
