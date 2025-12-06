#ifndef PEDIDOREPOSITORY_H
#define PEDIDOREPOSITORY_H

#include "common/models/PedidoMesa.h"
#include "common/models/InfoPlatoPrioridad.h"
#include <unordered_map>
#include <queue>

using ColaPrioridadPlatos = std::priority_queue<InfoPlatoPrioridad>;

class PedidoRepository {
public:
    PedidoRepository();

    long long generarNuevoIdPedido();
    long long generarNuevoIdInstancia();

    std::unordered_map<long long, PedidoMesa>& pedidos();
    const std::unordered_map<long long, PedidoMesa>& pedidos() const;

    std::queue<long long>& colaManagerChef();

    std::unordered_map<std::string, ColaPrioridadPlatos>& colasPorEstacion();
    const std::unordered_map<std::string, ColaPrioridadPlatos>& colasPorEstacion() const;

    void incrementarConteoRanking(int idPlato);
    void decrementarConteoRanking(int idPlato);
    std::unordered_map<int,int> obtenerConteoRanking() const;

private:
    long long m_siguienteIdPedido;
    long long m_siguienteIdInstanciaPlato;

    std::unordered_map<long long, PedidoMesa> m_pedidosActivos;
    std::queue<long long> m_colaManagerChef;
    std::unordered_map<std::string, ColaPrioridadPlatos> m_colasPorEstacion;
};

#endif
