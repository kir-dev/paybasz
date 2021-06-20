package hu.schbme.paybasz.station.controller;

import hu.schbme.paybasz.station.config.AppUtil;
import hu.schbme.paybasz.station.service.GatewayService;
import hu.schbme.paybasz.station.service.LoggingService;
import hu.schbme.paybasz.station.service.TransactionService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.ResponseBody;

import javax.servlet.http.HttpServletResponse;

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

    @GetMapping("/transactions")
    public String transactions(Model model) {
        model.addAttribute("transactions", system.getAllTransactions());
        return "transactions";
    }

    @GetMapping("/export")
    public String export() {
        return "export";
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
