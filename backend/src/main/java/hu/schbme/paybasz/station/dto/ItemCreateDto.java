package hu.schbme.paybasz.station.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class ItemCreateDto {

    private Integer id = null;
    private String name;
    private String quantity;
    private String code;
    private String abbreviation;
    private Integer price;

}
