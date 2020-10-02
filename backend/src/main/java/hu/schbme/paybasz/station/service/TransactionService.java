package hu.schbme.paybasz.station.service;

import hu.schbme.paybasz.station.dto.AccountCreateDto;
import hu.schbme.paybasz.station.dto.PaymentStatus;
import hu.schbme.paybasz.station.model.AccountEntity;
import hu.schbme.paybasz.station.model.TransactionEntity;
import hu.schbme.paybasz.station.repo.AccountRepository;
import hu.schbme.paybasz.station.repo.TransactionRepository;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.Optional;
import java.util.stream.Collectors;
import java.util.stream.Stream;

import static hu.schbme.paybasz.station.service.GatewayService.WEB_TERMINAL_NAME;

@Slf4j
@Service
public class TransactionService {

    @Autowired
    private TransactionRepository transactions;

    @Autowired
    private AccountRepository accounts;

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

        AccountEntity accountEntity = possibleAccount.get();
        if (!accountEntity.isAllowed()) {
            logger.failure("Sikertelen fizetés: <badge>" + accountEntity.getName() + "</badge>  <color>le van tiltva</color>");
            return PaymentStatus.CARD_REJECTED;
        }

        if (accountEntity.getBalance() - amount < accountEntity.getMinimumBalance()) {
            logger.failure("Sikertelen fizetés: <color>" + accountEntity.getName() + ", nincs elég fedezet</color>");
            return PaymentStatus.NOT_ENOUGH_CASH;
        }

        TransactionEntity transaction = new TransactionEntity(null, System.currentTimeMillis(), card, accountEntity.getId(),
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
    public boolean addMoneyToAccount(Long accountId, int amount) {
        Optional<AccountEntity> possibleAccount = this.accounts.findById(accountId);
        if (possibleAccount.isEmpty()) {
            logger.failure("Sikertelen egyenleg feltöltés: <color>felhasználó nem található</color>");
            return false;
        }

        AccountEntity accountEntity = possibleAccount.get();
        TransactionEntity transaction = new TransactionEntity(null, System.currentTimeMillis(), "NO-CARD-USED", -1L,
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
        accounts.save(new AccountEntity(null, name, card, phone, email, amount, minAmount, allowed));
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
        return accounts.findAll();
    }

    @Transactional(readOnly = true)
    public Optional<AccountEntity> getAccount(Long accountId) {
        return accounts.findById(accountId);
    }

    @Transactional(readOnly = false)
    public void setAllowed(Long accountId, boolean allow) {
        Optional<AccountEntity> user = accounts.findById(accountId);
        if (user.isPresent()) {
            user.get().setAllowed(allow);
            accounts.save(user.get());
        }
    }

    @Transactional(readOnly = false)
    public boolean modifyAccount(AccountCreateDto acc) {
        Optional<AccountEntity> cardCheck = accounts.findByCard(acc.getCard());
        Optional<AccountEntity> user = accounts.findById(acc.getId());
        if (user.isPresent()) {
            if (acc.getCard().length() > 24 && cardCheck.isPresent() && !cardCheck.get().getId().equals(user.get().getId()))
                return false;
            user.get().setName(acc.getName());
            user.get().setEmail(acc.getEmail());
            user.get().setPhone(acc.getPhone());
            user.get().setCard(acc.getCard());
            user.get().setMinimumBalance((acc.getLoan() == null || acc.getLoan() < 0) ? 0 : -acc.getLoan());
            logger.action("<color>" + user.get().getName() + "</color> adatai módosultak");
            accounts.save(user.get());
        }
        return true;
    }

    @Transactional(readOnly = false)
    public boolean createAccount(AccountCreateDto acc) {
        if (acc.getCard().length() > 24 && accounts.findByCard(acc.getCard()).isPresent())
            return false;

        AccountEntity account = new AccountEntity();
        account.setName(acc.getName());
        account.setEmail(acc.getEmail());
        account.setPhone(acc.getPhone());
        account.setCard(acc.getCard());
        account.setMinimumBalance((acc.getLoan() == null || acc.getLoan() < 0) ? 0 : -acc.getLoan());
        account.setAllowed(true);
        logger.note("<badge>" + account.getName() + "</badge> regisztrálva");
        accounts.save(account);
        return true;
    }

    @Transactional(readOnly = false)
    public PaymentStatus createTransactionToSystem(Long accountId, Integer amount) {
        Optional<AccountEntity> possibleAccount = this.accounts.findById(accountId);
        if (possibleAccount.isEmpty()) {
            logger.failure("Sikertelen fizetés: <color>felhasználó nem található</color>");
            return PaymentStatus.VALIDATION_ERROR;
        }

        AccountEntity accountEntity = possibleAccount.get();
        if (accountEntity.getBalance() - amount < accountEntity.getMinimumBalance()) {
            logger.failure("Sikertelen fizetés: <color>" + accountEntity.getName() + ", nincs elég fedezet</color>");
            return PaymentStatus.NOT_ENOUGH_CASH;
        }

        TransactionEntity transaction = new TransactionEntity(null, System.currentTimeMillis(), "NO-CARD-USED", accountEntity.getId(),
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
        return "id;name;email;phone;card;balance;minimumBalance;allowedToPay"
                + System.lineSeparator()
                + accounts.findAllByOrderById().stream()
                .map(it -> Stream.of("" + it.getId(), it.getName(), it.getEmail(), it.getPhone(), it.getCard(),
                        "" + it.getBalance(), "" + it.getMinimumBalance(), "" + it.isAllowed())
                        .map(attr -> attr.replaceAll(";", "\\;"))
                        .collect(Collectors.joining(";")))
                .collect(Collectors.joining(System.lineSeparator()));
    }

    @Transactional(readOnly = true)
    public String exportTransactions() {
        return "id;timestamp;time;sender;receiver;amount;card;description;message;senderId;paymentOrUpload"
                + System.lineSeparator()
                + transactions.findAllByOrderById().stream()
                .map(it -> Stream.of("" + it.getId(), "" + it.getTime(), it.formattedTime(), it.getCardHolder(), it.getReceiver(), "" + it.getAmount(),
                        it.getCardId(), it.getPaymentDescription(), it.getMessage(), "" + it.getAmount(), "" + it.isRegular())
                        .map(attr -> attr.replaceAll(";", "\\;"))
                        .collect(Collectors.joining(";")))
                .collect(Collectors.joining(System.lineSeparator()));
    }
}
