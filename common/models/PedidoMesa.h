#ifndef PEDIDOMESA_H
#define PEDIDOMESA_H

#include "PlatoInstancia.h"
#include "Estados.h"
#include <vector>
#include <string>
#include <chrono>

struct PedidoMesa {
  long long id_pedido;
  int numero_mesa;
  int id_recepcionista;

  EstadoPedido estado_general;
  std::chrono::system_clock::time_point timestamp_creacion;
  std::vector<PlatoInstancia> platos;
};

#endif