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

void PedidoRepository::incrementarConteoRanking(int idPlato) {
    m_conteoPlatosRanking[idPlato]++;
}

void PedidoRepository::decrementarConteoRanking(int idPlato) {
    auto it = m_conteoPlatosRanking.find(idPlato);
    if (it == m_conteoPlatosRanking.end()) return;

    it->second--;
    if (it->second <= 0) m_conteoPlatosRanking.erase(it);
}

std::unordered_map<int,int> PedidoRepository::obtenerConteoRanking() const {
    return m_conteoPlatosRanking;
}
