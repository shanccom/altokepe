#ifndef GESTOR_PEDIDOS_H
#define GESTOR_PEDIDOS_H

#include <QJsonArray>

class GestorPedidos {
public:
    void cargarDesdeHistorial();
    void agregarEntrada(const QJsonArray& platos);
};

extern GestorPedidos gestorPedidos;

#endif // GESTOR_PEDIDOS_H
