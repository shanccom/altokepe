#ifndef ESTADOS_H
#define ESTADOS_H

enum class EstadoPlato {
  EN_ESPERA,
  EN_PROGRESO,
  FINALIZADO,
  CANCELADO,
  ENTREGADO
};

enum class EstadoPedido {
  PENDIENTE,
  EN_PROGRESO,
  COMPLETADO,
  CANCELADO
};

enum class TipoActor {
  DESCONOCIDO,
  RECEPCIONISTA,
  MANAGER_CHEF,
  ESTACION_COCINA,
  RANKING
};

#endif