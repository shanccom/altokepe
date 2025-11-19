#ifndef ESTADOS_H
#define ESTADOS_H

// Estado de un plato individual en el flujo de la cocina.
enum class EstadoPlato {
  EN_ESPERA,
  EN_PROGRESO,
  FINALIZADO,
  CANCELADO,
  ENTREGADO
};

// Estado general de un pedido completo.
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