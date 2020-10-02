package hu.schbme.paybasz.station.error;

import org.springframework.http.HttpStatus;
import org.springframework.web.bind.annotation.ResponseStatus;

@ResponseStatus(code = HttpStatus.UNAUTHORIZED)
public class UnauthorizedGateway extends RuntimeException {

    public UnauthorizedGateway() {
    }

    public UnauthorizedGateway(String message) {
        super(message);
    }

}
