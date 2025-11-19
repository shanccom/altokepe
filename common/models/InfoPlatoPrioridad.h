#ifndef INFOPLATOPRIORIDAD_H
#define INFOPLATOPRIORIDAD_H

#include <functional>

struct InfoPlatoPrioridad {
  long long id_pedido;
  long long id_instancia_plato;
  double score_prioridad; 

  // Comparador para que std::priority_queue se comporte como un MinHeap.
  // Se define aquí, pero usaremos std::greater en la declaración de la cola.
  bool operator>(const InfoPlatoPrioridad& other) const {
    return this->score_prioridad > other.score_prioridad;
  }
};

#endif