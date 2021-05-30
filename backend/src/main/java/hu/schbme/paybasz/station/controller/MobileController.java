package hu.schbme.paybasz.station.controller;

import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;

@SuppressWarnings("SpellCheckingInspection")
@Slf4j
@Controller
@RequestMapping("/mobile")
public class MobileController {

    @GetMapping("/")
    public String index() {
        return "mobile/index";
    }


}
