#include "PedidoRepository.h"

PedidoRepository::PedidoRepository()
    : m_siguienteIdPedido(1),
      m_siguienteIdInstanciaPlato(1) {}

long long PedidoRepository::generarNuevoIdPedido() {
    return m_siguienteIdPedido++;
}

long long PedidoRepository::generarNuevoIdInstancia() {
    return m_siguienteIdInstanciaPlato++;
}

std::unordered_map<long long, PedidoMesa>& PedidoRepository::pedidos() {
    return m_pedidosActivos;
}

const std::unordered_map<long long, PedidoMesa>& PedidoRepository::pedidos() const {
    return m_pedidosActivos;
}

std::queue<long long>& PedidoRepository::colaManagerChef() {
    return m_colaManagerChef;
}

std::unordered_map<std::string, ColaPrioridadPlatos>& PedidoRepository::colasPorEstacion() {
    return m_colasPorEstacion;
}

const std::unordered_map<std::string, ColaPrioridadPlatos>& PedidoRepository::colasPorEstacion() const {
    return m_colasPorEstacion;
}
