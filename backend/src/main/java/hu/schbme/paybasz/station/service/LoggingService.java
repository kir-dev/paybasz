package hu.schbme.paybasz.station.service;

import hu.schbme.paybasz.station.dto.LogSeverity;
import hu.schbme.paybasz.station.dto.LoggingEntry;
import org.springframework.stereotype.Service;

import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.stream.Collectors;

@Service
public class LoggingService {

    List<LoggingEntry> entries = Collections.synchronizedList(new LinkedList<>());

    public void note(String message) {
        entries.add(new LoggingEntry(System.currentTimeMillis(), LogSeverity.NOTE, message));
    }

    public void success(String message) {
        entries.add(new LoggingEntry(System.currentTimeMillis(), LogSeverity.SUCCESS, message));
    }

    public void failure(String message) {
        entries.add(new LoggingEntry(System.currentTimeMillis(), LogSeverity.FAILURE, message));
    }

    public void error(String message) {
        entries.add(new LoggingEntry(System.currentTimeMillis(), LogSeverity.ERROR, message));
    }

    public void action(String message) {
        entries.add(new LoggingEntry(System.currentTimeMillis(), LogSeverity.ACTION, message));
    }

    public void serverInfo(String message) {
        entries.add(new LoggingEntry(System.currentTimeMillis(), LogSeverity.SERVER_INFO, message));
    }

    public void serverWarning(String message) {
        entries.add(new LoggingEntry(System.currentTimeMillis(), LogSeverity.SERVER_WARNING, message));
    }

    public List<LoggingEntry> getEntries() {
        return entries;
    }

    public String exportLogs() {
        return "timestamp;time;severity;message"
                + System.lineSeparator()
                + entries.stream()
                .map(it -> String.join(";", "" + it.getTimestamp(),
                        it.getFormattedDate().replace("&nbsp;", " "), it.getSeverity().name(),
                        it.getMarkdownMessage()))
                .collect(Collectors.joining(System.lineSeparator()));
    }

}
