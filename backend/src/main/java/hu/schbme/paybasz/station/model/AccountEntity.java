package hu.schbme.paybasz.station.model;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import javax.persistence.*;

@Data
@Entity
@Table(name = "accounts")
@NoArgsConstructor
@AllArgsConstructor
public class AccountEntity {

    @Id
    @Column
    @GeneratedValue
    private Long id;

    @Column(nullable = false)
    private String name;

    @Column(nullable = false)
    private String card;

    @Column(nullable = false)
    private String phone;

    @Column(nullable = false)
    private String email;

    @Column(nullable = false)
    private int balance;

    @Column(nullable = false)
    private int minimumBalance;

    @Column(nullable = false)
    private boolean allowed;

    @Transient
    public int getMaxLoan() {
        return -minimumBalance;
    }

    @Transient
    public String getFormattedCard() {
        return card.length() < 9 ? card : card.substring(0, 9);
    }

}
