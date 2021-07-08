package hu.schbme.paybasz.station.config;

import hu.schbme.paybasz.station.dto.GatewayCreateDto;
import hu.schbme.paybasz.station.service.GatewayService;
import hu.schbme.paybasz.station.service.TransactionService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Profile;

import javax.annotation.PostConstruct;

@SuppressWarnings("SpellCheckingInspection")
@Profile("test")
@Configuration
public class TestConfig {

    @Autowired
    private TransactionService system;

    @Autowired
    private GatewayService gatewayService;

    @PostConstruct
    public void init() {
        system.createAccount("Test Name", "test@test.com", "+36301234567", "AABBCCDD00110", 2500, 0, false);
        system.createAccount("Another test", "another@test.com", "+36307654321", "AABBCCEE214234", 100, -1000, true);

        system.createItem("Napi menü", "3dl", "103", "Napi menu 30", 750, false);
        system.createItem("Napi menü", "5dl", "105", "Napi menu 50", 750, true);
        system.createItem("Jäger meister", "2cl", "202", "Jager 2", 300, true);
        system.createItem("Jäger meister", "4cl", "204", "Jager 4", 600, true);
        system.createItem("Jäger meister", "8cl", "208", "Jager 8", 1200, false);
        system.createItem("Unicum", "2cl", "302", "Unicum 2", 320, true);
        system.createItem("Unicum", "4cl", "304", "Unicum 4", 640, false);
        system.createItem("Unicum", "8cl", "308", "Unicum 8", 1280, true);

        system.createItem("Unicum duplicate", "2cl", "302", "Unicum 2", 2320, true);
        system.createItem("Unicum duplicate", "4cl", "304", "Unicum 2", 2640, false);
        system.createItem("Unicum duplicate", "8cl", "308", "Unicum 2", 21280, true);

        gatewayService.createGateway(new GatewayCreateDto(0, "Name", "token", "physical"));
        gatewayService.appendReading("Name", "0123456789012345678901234567890123456789012345678901234567890123");
        gatewayService.appendReading("Name", "fdgdfsgfdgfdghuwioqbbvjasdzvuhfnbvh4738gz9WVBFKgevjhjkskdfkl01sd");
        gatewayService.appendReading("Name", "klfnkbaioubdxynjlubiogafgkaljgbas3fdsfsdfhtropw4nfjfvuizao8901ds");
    }

}
