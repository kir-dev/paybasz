package hu.schbme.paybasz.station.config;

import hu.schbme.paybasz.station.service.TransactionService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Profile;

import javax.annotation.PostConstruct;

@Profile("test")
@Configuration
public class TestConfig {

    @Autowired
    private TransactionService system;

    @PostConstruct
    public void init() {
        system.createTestAccount("Test Name", "test@test.com", "+36301234567", "AABBCCDD00110", 2500, 0, false);
        system.createTestAccount("Another test", "another@test.com", "+36307654321", "AABBCCEE214234", 100, -1000, true);
    }

}
