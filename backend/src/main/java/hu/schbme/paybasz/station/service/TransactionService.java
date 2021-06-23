package hu.schbme.paybasz.station.service;

import hu.schbme.paybasz.station.dto.AccountCreateDto;
import hu.schbme.paybasz.station.dto.ItemCreateDto;
import hu.schbme.paybasz.station.dto.ItemQueryResult;
import hu.schbme.paybasz.station.dto.PaymentStatus;
import hu.schbme.paybasz.station.model.AccountEntity;
import hu.schbme.paybasz.station.model.ItemEntity;
import hu.schbme.paybasz.station.model.TransactionEntity;
import hu.schbme.paybasz.station.repo.AccountRepository;
import hu.schbme.paybasz.station.repo.ItemRepository;
import hu.schbme.paybasz.station.repo.TransactionRepository;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.*;
import java.util.stream.Collectors;
import java.util.stream.Stream;

import static hu.schbme.paybasz.station.service.GatewayService.WEB_TERMINAL_NAME;

@SuppressWarnings({"DefaultAnnotationParam", "SpellCheckingInspection"})
@Slf4j
@Service
public class TransactionService {

    @Autowired
    private TransactionRepository transactions;

    @Autowired
    private AccountRepository accounts;

    @Autowired
    private ItemRepository items;

    @Autowired
    private LoggingService logger;

    @Transactional(readOnly = true)
    public Optional<AccountEntity> getAccountByCard(String card) {
        return accounts.findByCard(card);
    }

    @Transactional(readOnly = false)
    public PaymentStatus proceedPayment(String card, int amount, String message, String gateway) {
        Optional<AccountEntity> possibleAccount = this.accounts.findByCard(card);
        if (possibleAccount.isEmpty()) {
            logger.failure("Sikertelen fizetés: <color>kártya nem található</color>");
            return PaymentStatus.VALIDATION_ERROR;
        }

        var accountEntity = possibleAccount.get();
        if (!accountEntity.isAllowed()) {
            logger.failure("Sikertelen fizetés: <badge>" + accountEntity.getName() + "</badge>  <color>le van tiltva</color>");
            return PaymentStatus.CARD_REJECTED;
        }

        if (accountEntity.getBalance() - amount < accountEntity.getMinimumBalance()) {
            logger.failure("Sikertelen fizetés: <color>" + accountEntity.getName() + ", nincs elég fedezet</color>");
            return PaymentStatus.NOT_ENOUGH_CASH;
        }

        var transaction = new TransactionEntity(null, System.currentTimeMillis(), card, accountEntity.getId(),
                accountEntity.getName(), accountEntity.getName() + " payed " + amount + " with message: " + message,
                amount, message, gateway, "SYSTEM", true);
        accountEntity.setBalance(accountEntity.getBalance() - amount);
        accounts.save(accountEntity);
        transactions.save(transaction);
        log.info("Payment proceed: " + transaction.getId() + " with amount: " + transaction.getAmount() + " at gateway: " + transaction.getGateway());
        logger.success("<badge>" + accountEntity.getName() + "</badge> sikeres fizetés: <color>" + amount + " JMF</color>");
        return PaymentStatus.ACCEPTED;
    }

    @Transactional(readOnly = false)
    public boolean addMoneyToAccount(Integer accountId, int amount) {
        Optional<AccountEntity> possibleAccount = this.accounts.findById(accountId);
        if (possibleAccount.isEmpty()) {
            logger.failure("Sikertelen egyenleg feltöltés: <color>felhasználó nem található</color>");
            return false;
        }

        var accountEntity = possibleAccount.get();
        var transaction = new TransactionEntity(null, System.currentTimeMillis(), "NO-CARD-USED", -1,
                "SYSTEM", "SYSTEM payed " + amount + " with message: WEBTERM",
                amount, "WEBTERM", WEB_TERMINAL_NAME, accountEntity.getName(), false);

        accountEntity.setBalance(accountEntity.getBalance() + amount);
        accounts.save(accountEntity);
        transactions.save(transaction);
        log.info(transaction.getAmount() + " money added to: " + accountEntity.getName());
        logger.success("<badge>" + accountEntity.getName() + "</badge> számlájára feltöltve: <color>" + amount + " JMF</color>");
        return true;
    }

    @Transactional(readOnly = false)
    public void createTestAccount(String name, String email, String phone, String card, int amount, int minAmount, boolean allowed) {
        card = card.toUpperCase();
        log.info("New user was created with card: " + card);
        logger.note("<badge>" + name + "</badge> regisztrálva");
        accounts.save(new AccountEntity(null, name, card, phone, email, amount, minAmount, allowed, false, ""));
    }

    @Transactional(readOnly = false)
    public void createTestItem(String name, String quantity, String code, String abbreviation, int price, boolean active) {
        log.info("New item was created: " + name + " (" + quantity + ") " + price + " JMF");
        logger.note("<badge>" + name + "</badge> termék hozzáadva");
        items.save(new ItemEntity(null, name, quantity, code, abbreviation, price, active));
    }

    @Transactional(readOnly = true)
    public Iterable<TransactionEntity> getAllTransactions() {
        return transactions.findAll();
    }

    @Transactional(readOnly = true)
    public long getUserCount() {
        return accounts.count();
    }

    @Transactional(readOnly = true)
    public long getTransactionCount() {
        return transactions.count();
    }

    @Transactional(readOnly = true)
    public long getSumOfIncome() {
        return transactions.findAllByRegularIsTrue().stream()
                .mapToInt(TransactionEntity::getAmount)
                .sum();
    }

    @Transactional(readOnly = true)
    public long getSumOfLoans() {
        return Math.abs(accounts.findAllByBalanceLessThan(0).stream()
                .mapToInt(AccountEntity::getBalance)
                .sum());
    }

    @Transactional(readOnly = true)
    public long getSumOfBalances() {
        return accounts.findAllByBalanceGreaterThan(0).stream()
                .mapToInt(AccountEntity::getBalance)
                .sum();
    }

    @Transactional(readOnly = true)
    public long getSumOfPayIns() {
        return transactions.findAllByRegularIsFalse().stream()
                .mapToInt(TransactionEntity::getAmount)
                .sum();
    }

    @Transactional(readOnly = true)
    public Iterable<AccountEntity> getAllAccounts() {
        final var all = accounts.findAll();
        all.sort(Comparator.comparing(AccountEntity::getName));
        return all;
    }

    @Transactional(readOnly = true)
    public Optional<AccountEntity> getAccount(Integer accountId) {
        return accounts.findById(accountId);
    }

    @Transactional(readOnly = false)
    public void setAccountAllowed(Integer accountId, boolean allow) {
        accounts.findById(accountId).ifPresent(accountEntity -> {
            accountEntity.setAllowed(allow);
            accounts.save(accountEntity);
        });
    }

    @Transactional(readOnly = false)
    public void setAccountProcessed(Integer accountId, boolean processed) {
        accounts.findById(accountId).ifPresent(accountEntity -> {
            accountEntity.setProcessed(processed);
            accounts.save(accountEntity);
        });
    }

    @Transactional(readOnly = false)
    public void setItemActive(Integer itemId, boolean activate) {
        items.findById(itemId).ifPresent(itemEntity -> {
            itemEntity.setActive(activate);
            items.save(itemEntity);
        });
    }

    @Transactional(readOnly = false)
    public boolean modifyAccount(AccountCreateDto acc) {
        Optional<AccountEntity> cardCheck = acc.getCard().length() > 24 ? accounts.findByCard(acc.getCard()) : Optional.empty();
        Optional<AccountEntity> user = accounts.findById(acc.getId());
        if (user.isPresent()) {
            final var account = user.get();
            if (acc.getCard().length() > 24 && cardCheck.isPresent() && !cardCheck.get().getId().equals(account.getId()))
                return false;

            account.setName(acc.getName());
            account.setEmail(acc.getEmail());
            account.setPhone(acc.getPhone());
            account.setCard(acc.getCard());
            account.setComment(acc.getComment());
            account.setMinimumBalance((acc.getLoan() == null || acc.getLoan() < 0) ? 0 : -acc.getLoan());
            logger.action("<color>" + account.getName() + "</color> adatai módosultak");
            accounts.save(account);
        }
        return true;
    }

    @Transactional(readOnly = false)
    public boolean createAccount(AccountCreateDto acc) {
        if (acc.getCard().length() > 24 && accounts.findByCard(acc.getCard()).isPresent())
            return false;

        var account = new AccountEntity();
        account.setName(acc.getName());
        account.setEmail(acc.getEmail());
        account.setPhone(acc.getPhone());
        account.setCard(acc.getCard());
        account.setComment(acc.getComment());
        account.setMinimumBalance((acc.getLoan() == null || acc.getLoan() < 0) ? 0 : -acc.getLoan());
        account.setAllowed(true);
        logger.note("<badge>" + account.getName() + "</badge> regisztrálva");
        accounts.save(account);
        return true;
    }

    @Transactional(readOnly = false)
    public PaymentStatus createTransactionToSystem(Integer accountId, Integer amount) {
        Optional<AccountEntity> possibleAccount = this.accounts.findById(accountId);
        if (possibleAccount.isEmpty()) {
            logger.failure("Sikertelen fizetés: <color>felhasználó nem található</color>");
            return PaymentStatus.VALIDATION_ERROR;
        }

        var accountEntity = possibleAccount.get();
        if (accountEntity.getBalance() - amount < accountEntity.getMinimumBalance()) {
            logger.failure("Sikertelen fizetés: <color>" + accountEntity.getName() + ", nincs elég fedezet</color>");
            return PaymentStatus.NOT_ENOUGH_CASH;
        }

        var transaction = new TransactionEntity(null, System.currentTimeMillis(), "NO-CARD-USED", accountEntity.getId(),
                accountEntity.getName(), accountEntity.getName() + " payed " + amount + " with message: WEBTERM",
                amount, "WEBTERM", WEB_TERMINAL_NAME, "SYSTEM", true);

        accountEntity.setBalance(accountEntity.getBalance() - amount);
        accounts.save(accountEntity);
        transactions.save(transaction);
        log.info("Payment proceed: " + transaction.getId() + " with amount: " + transaction.getAmount() + " at gateway: " + transaction.getGateway());
        logger.success("<badge>" + accountEntity.getName() + "</badge> sikeres fizetés: <color>" + amount + " JMF</color>");
        return PaymentStatus.ACCEPTED;
    }

    @Transactional(readOnly = true)
    public String exportAccounts() {
        return "id;name;email;phone;card;balance;minimumBalance;allowedToPay;processed;comment"
                + System.lineSeparator()
                + accounts.findAllByOrderById().stream()
                .map(it -> Stream.of("" + it.getId(), it.getName(), it.getEmail(), it.getPhone(), it.getCard(),
                            "" + it.getBalance(), "" + it.getMinimumBalance(), "" + it.isAllowed(), "" + it.isProcessed(),
                            it.getComment())
                        .map(attr -> attr.replace(";", "\\;"))
                        .collect(Collectors.joining(";")))
                .collect(Collectors.joining(System.lineSeparator()));
    }

    @Transactional(readOnly = true)
    public String exportTransactions() {
        return "id;timestamp;time;sender;receiver;amount;card;description;message;senderId;paymentOrUpload"
                + System.lineSeparator()
                + transactions.findAllByOrderById().stream()
                .map(it -> Stream.of("" + it.getId(), "" + it.getTime(), it.formattedTime(), it.getCardHolder(),
                            it.getReceiver(), "" + it.getAmount(), it.getCardId(), it.getPaymentDescription(),
                            it.getMessage(), "" + it.getAmount(), "" + it.isRegular())
                        .map(attr -> attr.replace(";", "\\;"))
                        .collect(Collectors.joining(";")))
                .collect(Collectors.joining(System.lineSeparator()));
    }

    @Transactional(readOnly = true)
    public String exportItems() {
        return "id;name;quantity;code;abbreviation;price;active"
                + System.lineSeparator()
                + items.findAllByOrderById().stream()
                .map(it -> Stream.of("" + it.getId(), "" + it.getName(), it.getQuantity(), it.getCode(), it.getAbbreviation(),
                            "" + it.getPrice(), "" + it.isActive())
                        .map(attr -> attr.replace(";", "\\;"))
                        .collect(Collectors.joining(";")))
                .collect(Collectors.joining(System.lineSeparator()));
    }

    @Transactional(readOnly = true)
    public List<ItemEntity> getALlItems() {
        return items.findAll();
    }

    @Transactional(readOnly = true)
    public Optional<ItemEntity> getItem(Integer id) {
        return items.findById(id);
    }

    @Transactional(readOnly = false)
    public void modifyItem(ItemCreateDto itemDto) {
        Optional<ItemEntity> itemEntity = items.findById(itemDto.getId());
        if (itemEntity.isPresent()) {
            final var item = itemEntity.get();

            item.setCode(itemDto.getCode());
            item.setName(itemDto.getName());
            item.setQuantity(itemDto.getQuantity());
            item.setAbbreviation(itemDto.getAbbreviation());
            item.setPrice(itemDto.getPrice());
            logger.action("<color>" + item.getName() + "</color> termék adatai módosultak");
            items.save(item);
        }
    }

    @Transactional(readOnly = false)
    public void createItem(ItemCreateDto itemDto) {
        var item = new ItemEntity();
        item.setCode(itemDto.getCode());
        item.setName(itemDto.getName());
        item.setQuantity(itemDto.getQuantity());
        item.setAbbreviation(itemDto.getAbbreviation());
        item.setPrice(itemDto.getPrice());
        item.setActive(false);
        logger.note("<badge>" + item.getName() + "</badge> termék hozzáadva");
        items.save(item);
    }

    @Transactional(readOnly = true)
    public ItemQueryResult resolveItemQuery(String query) {
        if (query.startsWith("#"))
            query = query.substring(1);

        final String[] parts = query.split("\\*", 2);
        String code = parts[0];
        int amount = parts.length > 1 ? Integer.parseInt(parts[1]) : 1;

        return items.findAllByCodeAndActiveTrueOrderByPriceDesc(code)
                .stream().findFirst()
                .map(it -> new ItemQueryResult(true,
                it.getAbbreviation() + (amount > 1 ? ("x" + amount) : ""),
                it.getPrice() * amount))
                .orElseGet(() -> new ItemQueryResult(false, "not found", 0));
    }
}
