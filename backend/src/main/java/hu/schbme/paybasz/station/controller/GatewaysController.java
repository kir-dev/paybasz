package hu.schbme.paybasz.station.controller;

import hu.schbme.paybasz.station.dto.GatewayCreateDto;
import hu.schbme.paybasz.station.model.GatewayEntity;
import hu.schbme.paybasz.station.service.GatewayService;
import hu.schbme.paybasz.station.service.LoggingService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;

import java.util.Optional;

@Slf4j
@Controller
@RequestMapping("/admin")
public class GatewaysController {

    @Autowired
    private GatewayService gatewayService;

    @Autowired
    private LoggingService logger;

    @GetMapping("/gateways")
    public String gateways(Model model) {
        model.addAttribute("gateways", gatewayService.getAllGatewayInfo());
        return "gateways";
    }

    @GetMapping("/create-gateway")
    public String createGateway(Model model) {
        model.addAttribute("gateway", null);
        model.addAttribute("createMode", true);
        model.addAttribute("error", false);
        return "gateway-manipulate";
    }

    @PostMapping("/create-gateway")
    public String createGateway(GatewayCreateDto gatewayDto, Model model) {
        if (!gatewayService.createGateway(gatewayDto)) {
            model.addAttribute("gateway", gatewayDto);
            model.addAttribute("createMode", true);
            model.addAttribute("error", true);
            return "gateway-manipulate";
        }
        return "redirect:/admin/gateways";
    }

    @GetMapping("/modify-gateway/{gatewayId}")
    public String modifyGateway(@PathVariable Integer gatewayId, Model model) {
        Optional<GatewayEntity> gateway = gatewayService.getGateway(gatewayId);
        model.addAttribute("createMode", false);
        gateway.ifPresentOrElse(
                acc -> model.addAttribute("gateway", acc),
                () -> model.addAttribute("gateway", null));
        return "gateway-manipulate";
    }

    @PostMapping("/modify-gateway")
    public String modifyGateway(GatewayCreateDto gatewayDto, Model model) {
        if (gatewayDto.getId() == null)
            return "redirect:/admin/gateways";

        Optional<GatewayEntity> gateway = gatewayService.getGateway(gatewayDto.getId());
        if (gateway.isPresent() && !gatewayService.modifyGateway(gatewayDto)) {
            model.addAttribute("gateway", gatewayDto);
            model.addAttribute("createMode", false);
            model.addAttribute("error", true);
            return "gateway-manipulate";
        }
        return "redirect:/admin/gateways";
    }

}
