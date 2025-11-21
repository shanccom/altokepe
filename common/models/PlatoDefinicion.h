#ifndef PLATODEFINICION_H
#define PLATODEFINICION_H

#include <string>

// Representa la definición de un plato en el menú.
struct PlatoDefinicion {
  int id;
  std::string nombre;
  double costo;
  int tiempo_preparacion_estimado;
  std::string estacion;
};

#endif
