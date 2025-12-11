#ifndef ESTADOS_H
#define ESTADOS_H

enum class EstadoPlato {
  EN_ESPERA,
  EN_PROGRESO,
  PREPARANDO,
  FINALIZADO,
  CANCELADO,
  ENTREGADO,
  DEVUELTO
};

enum class EstadoPedido {
  PENDIENTE,
  EN_PROGRESO,
  LISTO,
  ENTREGADO,
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
