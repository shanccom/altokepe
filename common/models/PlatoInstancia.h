#ifndef PLATOINSTANCIA_H
#define PLATOINSTANCIA_H

#include "Estados.h"
#include <chrono>

struct PlatoInstancia {
  long long id_instancia; 
  int id_plato_definicion; 
  
  EstadoPlato estado;
  std::chrono::system_clock::time_point timestamp_creacion;
  std::chrono::system_clock::time_point timestamp_ultimo_cambio;
};

#endif
