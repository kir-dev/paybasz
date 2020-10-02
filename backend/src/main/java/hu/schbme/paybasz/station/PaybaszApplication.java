package hu.schbme.paybasz.station;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication
public class PaybaszApplication {

    public static final String VERSION = "1.0.0";

    public static void main(String[] args) {
        SpringApplication.run(PaybaszApplication.class, args);
    }

}
