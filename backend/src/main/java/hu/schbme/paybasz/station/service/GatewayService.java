package hu.schbme.paybasz.station.service;

import hu.schbme.paybasz.station.dto.GatewayInfo;
import hu.schbme.paybasz.station.model.Gateway;
import hu.schbme.paybasz.station.model.TransactionEntity;
import hu.schbme.paybasz.station.repo.TransactionRepository;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import javax.annotation.PostConstruct;
import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Deque;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.stream.Collectors;

import static java.util.function.Predicate.not;

@Slf4j
@Service
public class GatewayService {

    public static final String WEB_TERMINAL_NAME = "WebTerminal";

    private final ConcurrentMap<String, Gateway> gateways = new ConcurrentHashMap<>();

    @Autowired
    private LoggingService logger;

    @Autowired
    private TransactionRepository transactions;

    public List<GatewayInfo> getAllGatewayInfo() {
        return gateways.values().stream()
                .map(it -> new GatewayInfo(it.getName(), it.getLastPacket(), it.getLastReadings(),
                        readTxCount(it), readAllTraffic(it), it.isPhysical()))
                .collect(Collectors.toUnmodifiableList());
    }

    private long readAllTraffic(Gateway gw) {
        return transactions.findAllByGateway(gw.getName()).stream()
                .mapToInt(TransactionEntity::getAmount)
                .sum();
    }

    private long readTxCount(Gateway gw) {
        return transactions.countAllByGateway(gw.getName());
    }

    @Value("${paybasz.gateways.file:config/gateways.csv}")
    public String configFilePath;

    @PostConstruct
    public void init() throws IOException {
        if (!Files.exists(Path.of(configFilePath))) {
            Files.createDirectories(new File(configFilePath).getParentFile().toPath());
            Files.writeString(Path.of(configFilePath), "gatewayName;5e9795e3f3ab55e7790a6283507c085db0d764fc\n");
            log.info("Default gateways file was created to: " + Path.of(configFilePath).getFileName().toString());
            return;
        }
        gateways.put(WEB_TERMINAL_NAME, new Gateway(WEB_TERMINAL_NAME, "", false));
        gateways.putAll(Files.readAllLines(Path.of(configFilePath))
                .stream()
                .filter(not(String::isBlank))
                .map(line -> line.split(";"))
                .collect(Collectors.toMap(key -> key[0], value -> new Gateway(value[0], value[1], true))));

        gateways.forEach((name, token) -> log.info("Gateway '{}' registered with token: '{}'", name, token));
    }

    public boolean authorizeGateway(String name, String token) {
        if (!gateways.containsKey(name) || !gateways.get(name).isPhysical()) {
            log.warn("Unauthorized gateway '{}' with token '{}'", name, token);
            logger.failure("Nem jogoult terminál: <color>" + name + "</color>");
            return false;
        }
        return gateways.get(name).getToken().equals(token);
    }

    public void appendReading(String name, String card) {
        Deque<Gateway.CardReading> cardReadings = gateways.get(name).getLastReadings();
        if (cardReadings.size() >= 5)
            cardReadings.removeLast();
        cardReadings.addFirst(new Gateway.CardReading(card, System.currentTimeMillis()));
    }

    public void updateLastUsed(String name) {
        gateways.get(name).setLastPacket(System.currentTimeMillis());
    }
}
