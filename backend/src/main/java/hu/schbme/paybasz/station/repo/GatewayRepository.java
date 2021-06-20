package hu.schbme.paybasz.station.repo;

import hu.schbme.paybasz.station.model.GatewayEntity;
import org.springframework.data.repository.CrudRepository;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Optional;

@Repository
public interface GatewayRepository extends CrudRepository<GatewayEntity, Integer> {

    List<GatewayEntity> findAll();

    Optional<GatewayEntity> findByName(String name);

}
