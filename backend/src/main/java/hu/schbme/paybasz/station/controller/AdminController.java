package hu.schbme.paybasz.station.controller;

import hu.schbme.paybasz.station.config.AppUtil;
import hu.schbme.paybasz.station.dto.AccountCreateDto;
import hu.schbme.paybasz.station.dto.ItemCreateDto;
import hu.schbme.paybasz.station.dto.PaymentStatus;
import hu.schbme.paybasz.station.model.AccountEntity;
import hu.schbme.paybasz.station.model.ItemEntity;
import hu.schbme.paybasz.station.service.GatewayService;
import hu.schbme.paybasz.station.service.LoggingService;
import hu.schbme.paybasz.station.service.TransactionService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.*;

import javax.servlet.http.HttpServletResponse;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;

import static hu.schbme.paybasz.station.config.AppUtil.formatNumber;

@SuppressWarnings("SpellCheckingInspection")
@Slf4j
@Controller
@RequestMapping("/admin")
public class AdminController {

    public static final String DUPLICATE_CARD_ERROR = "DUPLICATE_CARD";

    @Autowired
    private TransactionService system;

    @Autowired
    private GatewayService gatewayService;

    @Autowired
    private LoggingService logger;

    @RequestMapping("/")
    public String index(Model model) {
        model.addAttribute("userCount", system.getUserCount());
        model.addAttribute("txCount", system.getTransactionCount());
        model.addAttribute("sumOfIncome", formatNumber(system.getSumOfIncome()));

        model.addAttribute("logs", logger.getEntries());

        model.addAttribute("sumOfLoans", formatNumber(system.getSumOfLoans()));
        model.addAttribute("sumOfBalances", formatNumber(system.getSumOfBalances()));
        model.addAttribute("sumOfPayIns", formatNumber(system.getSumOfPayIns()));
        return "analytics";
    }

    @GetMapping("/accounts")
    public String accounts(Model model) {
        model.addAttribute("accounts", system.getAllAccounts());
        return "accounts";
    }

    @GetMapping("/manual-transaction/{accountId}")
    public String manualTransaction(@PathVariable Integer accountId, Model model) {
        Optional<AccountEntity> account = system.getAccount(accountId);
        account.ifPresentOrElse(acc -> model.addAttribute("balance", acc.getBalance())
                    .addAttribute("loan", Math.abs(acc.getMinimumBalance()))
                    .addAttribute("name", acc.getName())
                    .addAttribute("max", acc.getBalance() >= 0 ? (acc.getBalance() + -acc.getMinimumBalance()) : (-acc.getMinimumBalance() + acc.getBalance()))
                    .addAttribute("id", acc.getId()),
                () -> model.addAttribute("balance", 0)
                    .addAttribute("loan", 0)
                    .addAttribute("name", "Nem található")
                    .addAttribute("max", 0)
                    .addAttribute("id", -1));
        return "manual";
    }

    @PostMapping("/manual-transaction")
    public String manualTransaction(@RequestParam Integer id, @RequestParam Integer money) {
        if (money == null || money < 0)
            return "redirect:/admin/manual-transaction/" + id;

        PaymentStatus result = system.createTransactionToSystem(id, money);
        if (result == PaymentStatus.ACCEPTED)
            return "redirect:/admin/manual-transaction-done?money=" + money;
        return "redirect:/admin/manual-transaction/" + id + "?failed=" + result.name();
    }

    @GetMapping("/manual-transaction-done")
    public String manualTransactionDone(@RequestParam Long money, Model model) {
        model.addAttribute("money", money);
        return "manual-done";
    }

    @GetMapping("/upload-money/{accountId}")
    public String uploadMoney(@PathVariable Integer accountId, Model model) {
        Optional<AccountEntity> account = system.getAccount(accountId);
        account.ifPresentOrElse(
                acc -> model.addAttribute("name", acc.getName()).addAttribute("id", acc.getId()),
                () -> model.addAttribute("name", "Nem található").addAttribute("id", -1));
        return "upload-money";
    }

    @PostMapping("/upload-money")
    public String uploadMoney(@RequestParam Integer id, @RequestParam Integer money) {
        if (money == null || money < 0)
            return "redirect:/admin/upload-money/" + id;

        if (system.addMoneyToAccount(id, money))
            return "redirect:/admin/upload-money-done?money=" + money;
        return "redirect:/admin/upload-money/" + id + "?failed=";
    }

    @GetMapping("/upload-money-done")
    public String uploadMoneyDone(@RequestParam Long money, Model model) {
        model.addAttribute("money", money);
        return "upload-money-done";
    }
    @GetMapping("/create-account")
    public String createUser(Model model) {
        model.addAttribute("acc", null);
        model.addAttribute("createMode", true);
        return "account-manipulate";
    }

    @PostMapping("/create-account")
    public String createUser(Model model, AccountCreateDto acc) {
        acc.setCard(acc.getCard().trim().toUpperCase());
        if (system.createAccount(acc)) {
            return "redirect:/admin/accounts";
        } else {
            model.addAttribute("acc", acc);
            model.addAttribute("error", DUPLICATE_CARD_ERROR);
            model.addAttribute("createMode", true);
            return "account-manipulate";
        }
    }

    @GetMapping("/modify-account/{accountId}")
    public String modifyAccount(@PathVariable Integer accountId, Model model) {
        Optional<AccountEntity> account = system.getAccount(accountId);
        model.addAttribute("createMode", false);
        account.ifPresentOrElse(
                acc -> model.addAttribute("acc", acc),
                () -> model.addAttribute("acc", null));
        return "account-manipulate";
    }

    @PostMapping("/modify-account")
    public String modifyAccount(Model model, AccountCreateDto acc) {
        if (acc.getId() == null)
            return "redirect:/admin/accounts";

        acc.setCard(acc.getCard().trim().toUpperCase());
        Optional<AccountEntity> account = system.getAccount(acc.getId());
        if (account.isPresent() && !system.modifyAccount(acc)) {
            model.addAttribute("createMode", false);
            model.addAttribute("acc", account.get());
            model.addAttribute("error", DUPLICATE_CARD_ERROR);
            return "account-manipulate";
        }
        return "redirect:/admin/accounts";
    }

    @GetMapping("/items")
    public String items(Model model) {
        final var items = system.getALlItems();
        model.addAttribute("items", items);
        model.addAttribute("invalid", items.stream()
                .filter(ItemEntity::isActive)
                .collect(Collectors.groupingBy(ItemEntity::getCode))
                .entrySet().stream()
                .filter(it -> it.getValue().size() > 1)
                .map(it -> "#" + it.getKey())
                .collect(Collectors.joining(", ")));

        return "items";
    }

    @GetMapping("/create-item")
    public String createItem(Model model) {
        model.addAttribute("item", null);
        model.addAttribute("createMode", true);
        return "item-manipulate";
    }

    @PostMapping("/create-item")
    public String createItem(ItemCreateDto itemDto) {
        itemDto.setAbbreviation(itemDto.getAbbreviation().trim().toUpperCase());
        system.createItem(itemDto);
        return "redirect:/admin/items";
    }

    @GetMapping("/modify-item/{itemId}")
    public String modifyItem(@PathVariable Integer itemId, Model model) {
        Optional<ItemEntity> item = system.getItem(itemId);
        model.addAttribute("createMode", false);
        item.ifPresentOrElse(
                acc -> model.addAttribute("item", acc),
                () -> model.addAttribute("item", null));
        return "item-manipulate";
    }

    @PostMapping("/modify-item")
    public String modifyItem(ItemCreateDto itemDto) {
        if (itemDto.getId() == null)
            return "redirect:/admin/accounts";

        itemDto.setAbbreviation(itemDto.getAbbreviation().trim().toUpperCase());
        Optional<ItemEntity> item = system.getItem(itemDto.getId());
        if (item.isPresent()) {
            system.modifyItem(itemDto);
        }
        return "redirect:/admin/items";
    }

    @PostMapping("/items/activate")
    public String activateItem(@RequestParam Integer id) {
        Optional<ItemEntity> item = system.getItem(id);
        item.ifPresent(it -> {
            system.setItemActive(id, true);
            logger.action("<color>" + it.getName() + "</color> termék rendelhető");
            log.info("Item purchase activated for " + it.getName() + " (" +it.getQuantity() + ")");
        });
        return "redirect:/admin/items";
    }

    @PostMapping("/items/deactivate")
    public String deactivateItem(@RequestParam Integer id) {
        Optional<ItemEntity> item = system.getItem(id);
        item.ifPresent(it -> {
            system.setItemActive(id, false);
            logger.failure("<color>" + it.getName() + "</color> termék nem redelhető");
            log.info("Item purchase deactivated for " + it.getName() + " (" +it.getQuantity() + ")");
        });
        return "redirect:/admin/items";
    }

    @GetMapping("/transactions")
    public String transactions(Model model) {
        model.addAttribute("transactions", system.getAllTransactions());
        return "transactions";
    }

    @GetMapping("/gateways")
    public String gateways(Model model) {
        model.addAttribute("gateways", gatewayService.getAllGatewayInfo());
        return "gateways";
    }

    @GetMapping("/export")
    public String export() {
        return "export";
    }

    @PostMapping("/allow")
    public String allowAccount(@RequestParam Integer id) {
        Optional<AccountEntity> account = system.getAccount(id);
        account.ifPresent(acc -> {
            system.setAccountAllowed(id, true);
            logger.action("<color>" + acc.getName() + "</color> tiltása feloldva");
            log.info("User purchases allowed for " + acc.getName());
        });
        return "redirect:/admin/accounts";
    }

    @PostMapping("/disallow")
    public String disallowAccount(@RequestParam Integer id) {
        Optional<AccountEntity> account = system.getAccount(id);
        account.ifPresent(acc -> {
            system.setAccountAllowed(id, false);
            logger.failure("<color>" + acc.getName() + "</color> letiltva");
            log.info("User purchases disallowed for " + acc.getName());
        });
        return "redirect:/admin/accounts";
    }

    @PostMapping("/set-processed")
    public String setProcessedAccount(@RequestParam Integer id) {
        Optional<AccountEntity> account = system.getAccount(id);
        account.ifPresent(acc -> {
            system.setAccountProcessed(id, true);
            logger.success("<color>" + acc.getName() + "</color> könyvelve");
            log.info("User status set processed for " + acc.getName());
        });
        return "redirect:/admin/accounts";
    }

    @PostMapping("/unset-processed")
    public String unsetProcessedAccount(@RequestParam Integer id) {
        Optional<AccountEntity> account = system.getAccount(id);
        account.ifPresent(acc -> {
            system.setAccountProcessed(id, false);
            logger.failure("<color>" + acc.getName() + "</color> könyvelési státusza: nincs könyvelve");
            log.info("User processed status unset for " + acc.getName());
        });
        return "redirect:/admin/accounts";
    }

    @GetMapping("/export/accounts")
    @ResponseBody
    public String exportAccounts(HttpServletResponse response) {
        response.setContentType("text/csv");
        response.setHeader("Content-Disposition", "attachment; filename=\"paybasz-accounts-"
                + AppUtil.DATE_TIME_FILE_FORMATTER.format(System.currentTimeMillis()) + ".csv\"");

        String csvExport = system.exportAccounts();
        logger.action("Felhasználók kiexportálva");
        return csvExport;
    }

    @GetMapping("/export/transactions")
    @ResponseBody
    public String exportTransactions(HttpServletResponse response) {
        response.setContentType("text/csv");
        response.setHeader("Content-Disposition", "attachment; filename=\"paybasz-transactions-"
                + AppUtil.DATE_TIME_FILE_FORMATTER.format(System.currentTimeMillis()) + ".csv\"");

        String csvExport = system.exportTransactions();
        logger.action("Tranzakciók kiexportálva");
        return csvExport;
    }

    @GetMapping("/export/logs")
    @ResponseBody
    public String exportLogs(HttpServletResponse response) {
        response.setContentType("text/csv");
        response.setHeader("Content-Disposition", "attachment; filename=\"paybasz-logs-"
                + AppUtil.DATE_TIME_FILE_FORMATTER.format(System.currentTimeMillis()) + ".csv\"");

        String csvExport = logger.exportLogs();
        logger.action("Eseménynapló kiexportálva");
        return csvExport;
    }

    @GetMapping("/export/items")
    @ResponseBody
    public String exportItems(HttpServletResponse response) {
        response.setContentType("text/csv");
        response.setHeader("Content-Disposition", "attachment; filename=\"paybasz-items-"
                + AppUtil.DATE_TIME_FILE_FORMATTER.format(System.currentTimeMillis()) + ".csv\"");

        String csvExport = system.exportItems();
        logger.action("Termék lista kiexportálva");
        return csvExport;
    }

}
