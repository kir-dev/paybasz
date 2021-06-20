package hu.schbme.paybasz.station.controller;

import hu.schbme.paybasz.station.dto.ItemCreateDto;
import hu.schbme.paybasz.station.model.ItemEntity;
import hu.schbme.paybasz.station.service.LoggingService;
import hu.schbme.paybasz.station.service.TransactionService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.*;

import java.util.Optional;
import java.util.stream.Collectors;

@SuppressWarnings("SpellCheckingInspection")
@Slf4j
@Controller
@RequestMapping("/admin")
public class ItemsController {

    @Autowired
    private TransactionService system;

    @Autowired
    private LoggingService logger;

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
        itemDto.setAbbreviation(itemDto.getAbbreviation().trim());
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
            return "redirect:/admin/items";

        itemDto.setAbbreviation(itemDto.getAbbreviation().trim());
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

}
