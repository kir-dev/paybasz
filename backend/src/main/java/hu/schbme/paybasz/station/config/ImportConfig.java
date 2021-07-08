package hu.schbme.paybasz.station.config;

import hu.schbme.paybasz.station.dto.GatewayCreateDto;
import hu.schbme.paybasz.station.service.GatewayService;
import hu.schbme.paybasz.station.service.TransactionService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Profile;

import javax.annotation.PostConstruct;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

@Profile("import")
@Configuration
public class ImportConfig {

    @Autowired
    private TransactionService system;

    @PostConstruct
    public void init() throws IOException {
        importAccounts();
        importItems();
    }

    void importAccounts() throws IOException {
        if (system.getAllAccounts().iterator().hasNext()) {
            System.out.println("There are already accounts in you db!");
            return;
        }
        System.out.println("Importing data/accounts.csv");

        final var data = Path.of("data", "accounts.csv");
        if (Files.exists(data)) {
            try (final var lines = Files.lines(data)) {
                lines.map(it -> it.split(";"))
                    // format: name; email; mobile
                    .forEach(it -> system.createAccount(it[0].trim(), it[1].trim(), it[2].trim(), "", 0, 0, true));
            }
        }
    }

    void importItems() throws IOException {
        if (system.getALlItems().iterator().hasNext()) {
            System.out.println("There are already items in you db!");
            return;
        }
        System.out.println("Importing data/items.csv");

        final var data = Path.of("data", "items.csv");
        if (Files.exists(data)) {
            try (final var lines = Files.lines(data)) {
                lines.map(it -> it.split(";"))
                        // format: name; quantity; code; short name; price int; active boolean
                        .forEach(it -> system.createItem(it[0].trim(), it[1].trim(), it[2].trim(), it[3].trim(), Integer.parseInt(it[4].trim()), Boolean.parseBoolean(it[5].trim())));
            }
        } else {
            System.out.println(data.toString() + " does not exists!");
        }
    }
}
