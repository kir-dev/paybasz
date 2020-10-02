package hu.schbme.paybasz.station.repo;

import hu.schbme.paybasz.station.model.AccountEntity;
import org.springframework.data.repository.CrudRepository;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Optional;
import java.util.stream.Stream;

@Repository
public interface AccountRepository extends CrudRepository<AccountEntity, Long> {

    Optional<AccountEntity> findByCard(String card);

    List<AccountEntity> findAllByBalanceLessThan(int zero);

    List<AccountEntity> findAllByBalanceGreaterThan(int zero);

    List<AccountEntity> findAllByOrderById();

}
