#include "LogicaNegocio.h"
#include "ManejadorCliente.h"
#include "common/network/Protocolo.h"
#include "common/models/Estados.h"
#include "common/network/SerializadorJSON.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <algorithm>

LogicaNegocio* LogicaNegocio::s_instance = nullptr;

LogicaNegocio::LogicaNegocio(QObject* parent) 
  : QObject(parent), 
    m_siguienteIdPedido(1), 
    m_siguienteIdInstanciaPlato(1) 
{
  cargarMenuDesdeArchivo(":/menu.json");
}

LogicaNegocio* LogicaNegocio::instance() {
  if (s_instance == nullptr) {
    s_instance = new LogicaNegocio();
  }
  return s_instance;
}

void LogicaNegocio::cargarMenuDesdeArchivo(const QString& rutaArchivo) {
  std::lock_guard<std::mutex> lock(m_mutex);
  QFile archivo(rutaArchivo);
  if (!archivo.open(QIODevice::ReadOnly)) {
    qCritical() << "No se pudo abrir el archivo de menú:" << rutaArchivo;
    return;
  }

  QByteArray data = archivo.readAll();
  QJsonDocument doc = QJsonDocument::fromJson(data);
  if (!doc.isArray()) {
    qCritical() << "El archivo de menú no es un array JSON válido.";
    return;
  }

  m_menu.clear();
  QJsonArray menuArray = doc.array();
  for (const QJsonValue& val : menuArray) {
    PlatoDefinicion plato = SerializadorJSON::jsonToPlatoDefinicion(val.toObject());
    m_menu[plato.id] = plato;
  }
  qInfo() << "Menú cargado desde" << rutaArchivo << "con" << m_menu.size() << "platos.";
}

void LogicaNegocio::registrarManejador(ManejadorCliente* manejador) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_manejadoresActivos.push_back(manejador);
}

void LogicaNegocio::eliminarManejador(ManejadorCliente* manejador) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_manejadoresActivos.erase(std::remove(m_manejadoresActivos.begin(),
        m_manejadoresActivos.end(), manejador), m_manejadoresActivos.end());
}

void LogicaNegocio::enviarEstadoInicial(ManejadorCliente* cliente) {
  std::lock_guard<std::mutex> lock(m_mutex);
  QJsonObject estado;

  TipoActor tipo = cliente->getTipoActor();

  if (tipo == TipoActor::MANAGER_CHEF) {
    //estado = getEstadoParaManager(true);
  } else if (tipo == TipoActor::RANKING) {
    estado = getEstadoParaRanking();
    emit enviarRespuesta(cliente, estado);
  } else if (tipo == TipoActor::ESTACION_COCINA) {
    //estado = getEstadoParaEstacion(cliente->getNombreEstacion().toStdString());
  } else if (tipo == TipoActor::RECEPCIONISTA) {
    QJsonObject mensaje;
    QJsonArray menuArray;

    for (const auto& par : m_menu) {
      const PlatoDefinicion& plato = par.second;
      menuArray.append(SerializadorJSON::platoDefinicionToJson(plato));
    }

    mensaje[Protocolo::EVENTO] = Protocolo::ACTUALIZACION_MENU;
    mensaje[Protocolo::DATA] = QJsonObject{ {"menu", menuArray} };

    emit enviarRespuesta(cliente, mensaje);
    return;
  }
}

QJsonObject LogicaNegocio::getEstadoParaRanking() {
  // Convertir Mapa a Vector para ordenar
  struct ItemRanking {
    QString nombre;
    int cantidad;
  };
  std::vector<ItemRanking> lista;

  for (auto const& [id, cantidad] : m_conteoPlatosRanking) {
    if (m_menu.find(id) != m_menu.end()) {
      lista.push_back({QString::fromStdString(m_menu[id].nombre), cantidad});
    }
  }

  // Ordenar (Mayor a menor cantidad)
  std::sort(lista.begin(), lista.end(), [](const ItemRanking& a, const ItemRanking& b) {
    return a.cantidad > b.cantidad;
  });

  // Construir JSON
  QJsonArray rankingArray;
  for (const auto& item : lista) {
    QJsonObject obj;
    obj["nombre"] = item.nombre;
    obj["cantidad"] = item.cantidad;
    rankingArray.append(obj);
  }

  QJsonObject mensaje;
  mensaje[Protocolo::EVENTO] = Protocolo::ACTUALIZACION_RANKING;
  mensaje[Protocolo::DATA] = QJsonObject{ {"ranking", rankingArray} };
  
  return mensaje;
}

void LogicaNegocio::registrarVenta(int idPlato) {
  QJsonObject rankingMsg;
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_conteoPlatosRanking[idPlato]++;
    rankingMsg = getEstadoParaRanking(); // Generar bajo lock
  }

  // Notificar a todos (Observer)
  emit enviarRespuesta(nullptr, rankingMsg);
}

void LogicaNegocio::procesarNuevoPedido(const QJsonObject& mensaje, ManejadorCliente* remitente) {
  std::lock_guard<std::mutex> lock(m_mutex);

  if (!mensaje.contains(Protocolo::DATA) || !mensaje[Protocolo::DATA].isObject()) {
    qWarning() << "NUEVO_PEDIDO sin data";
    return;
  }

  QJsonObject data = mensaje[Protocolo::DATA].toObject();

  if (!data.contains("platos") || !data["platos"].isArray()) {
    qWarning() << "NUEVO_PEDIDO sin platos";
    return;
  }

  long long idPedido = m_siguienteIdPedido++;

  PedidoMesa pedido;
  pedido.id_pedido = idPedido;
  pedido.estado_general = EstadoPedido::PENDIENTE;

  QJsonArray platos = data["platos"].toArray();

  for (auto plato : platos) {
    int idPlato = plato.toInt();

    if (m_menu.find(idPlato) == m_menu.end()) {
      qWarning() << "Plato inválido:" << idPlato;
      continue;
    }

    const PlatoDefinicion& platoDef = m_menu[idPlato];

    PlatoInstancia platoInst;
    platoInst.id_instancia = m_siguienteIdInstanciaPlato++;
    platoInst.id_plato_definicion = platoDef.id;
    platoInst.estado = EstadoPlato::EN_ESPERA;

    pedido.platos.push_back(platoInst);

    // Encolar tarea para cocina
    InfoPlatoPrioridad platoPrior(platoInst.id_instancia, platoDef.tiempo_preparacion_estimado);
    platoPrior.id_pedido = idPedido;
    m_colasPorEstacion[platoDef.estacion].push(platoPrior);
  }

  m_pedidosActivos[idPedido] = pedido;
  m_colaManagerChef.push(idPedido);

  QJsonObject msg;
  msg[Protocolo::EVENTO] = Protocolo::PEDIDO_REGISTRADO;
  msg["id_pedido"] = (int)idPedido;

  emit enviarRespuesta(remitente, msg);

  for (auto cli : m_manejadoresActivos)
    if (cli->getTipoActor() == TipoActor::MANAGER_CHEF)
      emit enviarRespuesta(cli, msg);

  qInfo() << "Pedido" << idPedido << "registrado.";
}

void LogicaNegocio::procesarPrepararPedido(const QJsonObject& mensaje, ManejadorCliente* remitente) {
  std::lock_guard<std::mutex> lock(m_mutex);

  ResultadoValidacion pedido = validarYObtenerPlato(mensaje, remitente, true);
  if (!pedido.exito) return;

  pedido.plato->estado = EstadoPlato::EN_PROGRESO;

  if (pedido.pedido->estado_general == EstadoPedido::PENDIENTE) {
    pedido.pedido->estado_general = EstadoPedido::EN_PROGRESO;
  }

  QJsonObject msg;
  msg[Protocolo::EVENTO] = Protocolo::PLATO_EN_PREPARACION;
  msg["id_pedido"] = (int)pedido.idPedido;
  msg["id_instancia"] = (int)pedido.idInstancia;

  for (auto cli : m_manejadoresActivos)
    emit enviarRespuesta(cli, msg);

  qInfo() << "Plato" << pedido.idInstancia << "del pedido" << pedido.idPedido << "pasó a EN_PREPARACION.";
}

void LogicaNegocio::procesarCancelarPedido(const QJsonObject& mensaje, ManejadorCliente* remitente) {
  std::lock_guard<std::mutex> lock(m_mutex);

  if (!mensaje.contains(Protocolo::DATA) || !mensaje[Protocolo::DATA].isObject()) {
    qWarning() << "CANCELAR_PEDIDO sin data";
    return;
  }

  QJsonObject data = mensaje[Protocolo::DATA].toObject();

  if (!data.contains("id_pedido")) {
    qWarning() << "CANCELAR_PEDIDO sin id_pedido";
    return;
  }

  long long idPedido = data["id_pedido"].toInt();

  if (m_pedidosActivos.find(idPedido) == m_pedidosActivos.end()) {
    qWarning() << "Pedido no existe:" << idPedido;
    return;
  }

  PedidoMesa& pedido = m_pedidosActivos[idPedido];
  pedido.estado_general = EstadoPedido::CANCELADO;

  for (auto& inst : pedido.platos) {
    inst.estado = EstadoPlato::CANCELADO;
  }

  QJsonObject msg;
  msg[Protocolo::EVENTO] = Protocolo::PEDIDO_CANCELADO;
  msg["id_pedido"] = (int)idPedido;

  // Notifica a todos los roles
  for (auto cli : m_manejadoresActivos)
    emit enviarRespuesta(cli, msg);

  qInfo() << "Pedido" << idPedido << "ha sido CANCELADO.";
}

void LogicaNegocio::procesarMarcarPlatoTerminado(const QJsonObject& mensaje, ManejadorCliente* remitente) {
  std::lock_guard<std::mutex> lock(m_mutex);

  ResultadoValidacion pedido = validarYObtenerPlato(mensaje, remitente, true);
  if (!pedido.exito) return;

  pedido.plato->estado = EstadoPlato::FINALIZADO;

  bool todoTerminado = true;
  for (const auto& inst : pedido.pedido->platos) {
    if (inst.estado != EstadoPlato::FINALIZADO && inst.estado != EstadoPlato::CANCELADO) {
      todoTerminado = false;
      break;
    }
  }

  if (todoTerminado) {
    pedido.pedido->estado_general = EstadoPedido::LISTO;
  }

  QJsonObject msg;
  msg[Protocolo::EVENTO] = Protocolo::PLATO_TERMINADO;
  msg["id_pedido"] = (int)pedido.idPedido;
  msg["id_instancia"] = (int)pedido.idInstancia;
  msg["pedido_listo"] = todoTerminado;

  for (auto cli : m_manejadoresActivos)
    emit enviarRespuesta(cli, msg);

  qInfo() << "Plato" << pedido.idInstancia << "terminado. Pedido"
          << pedido.idPedido << (todoTerminado ? "LISTO" : "AÚN EN PROCESO");
}

void LogicaNegocio::procesarConfirmarEntrega(const QJsonObject& mensaje, ManejadorCliente* remitente) {
  std::lock_guard<std::mutex> lock(m_mutex);

  if (!mensaje.contains(Protocolo::DATA) || !mensaje[Protocolo::DATA].isObject()) {
    qWarning() << "CONFIRMAR_ENTREGA sin data";
    return;
  }

  QJsonObject data = mensaje[Protocolo::DATA].toObject();

  if (!data.contains("id_pedido")) {
    qWarning() << "CONFIRMAR_ENTREGA sin id_pedido";
    return;
  }

  long long idPedido = data["id_pedido"].toInt();

  if (m_pedidosActivos.find(idPedido) == m_pedidosActivos.end()) {
    qWarning() << "Pedido no encontrado en CONFIRMAR_ENTREGA:" << idPedido;
    return;
  }

  PedidoMesa& pedido = m_pedidosActivos[idPedido];
  if (pedido.estado_general != EstadoPedido::LISTO) {
    qWarning() << "No se puede confirmar entrega: pedido no está LISTO. Estado actual:" << (int)pedido.estado_general;
    return;
  }

  pedido.estado_general = EstadoPedido::ENTREGADO;

  for (auto& inst : pedido.platos) {
    inst.estado = EstadoPlato::ENTREGADO;
  }

  QJsonObject msg;
  msg[Protocolo::EVENTO] = Protocolo::PEDIDO_ENTREGADO;
  msg["id_pedido"] = (int)idPedido;

  for (auto cli : m_manejadoresActivos) {
    emit enviarRespuesta(cli, msg);
  }

  // Actualizar ranking
  for (const auto& inst : pedido.platos) {
    m_conteoPlatosRanking[inst.id_plato_definicion]++;
  }

  qInfo() << "Pedido" << idPedido << "ENTREGADO correctamente.";
}

void LogicaNegocio::procesarDevolverPlato(const QJsonObject& mensaje, ManejadorCliente* remitente) {
  std::lock_guard<std::mutex> lock(m_mutex);

  ResultadoValidacion pedido = validarYObtenerPlato(mensaje, remitente, false);
  if (!pedido.exito) return;

  if (pedido.plato->estado != EstadoPlato::ENTREGADO) {
    qWarning() << "No se puede devolver un plato que NO está ENTREGADO.";
    return;
  }

  pedido.plato->estado = EstadoPlato::DEVUELTO;

  // Obtenemos info extra necesaria para la lógica de re-encolado
  const PlatoDefinicion& platoDef = m_menu[pedido.plato->id_plato_definicion];
  std::string estacionObjetivo = platoDef.estacion;

  pedido.pedido->estado_general = EstadoPedido::EN_PROGRESO;

  InfoPlatoPrioridad platoPrior(pedido.idInstancia, platoDef.tiempo_preparacion_estimado);
  platoPrior.id_pedido = pedido.idPedido;
  m_colasPorEstacion[estacionObjetivo].push(platoPrior);

  // Ajuste de ranking
  m_conteoPlatosRanking[pedido.plato->id_plato_definicion]--;

  QJsonObject msg;
  msg[Protocolo::EVENTO] = Protocolo::PLATO_DEVUELTO;
  msg["id_pedido"] = (int)pedido.idPedido;
  msg["id_instancia"] = (int)pedido.idInstancia;
  msg["estacion"] = QString::fromStdString(estacionObjetivo);

  for (auto cli : m_manejadoresActivos)
    emit enviarRespuesta(cli, msg);

  qInfo() << "Plato" << pedido.idInstancia << "del pedido" << pedido.idPedido << "ha sido DEVUELTO.";
}

LogicaNegocio::ResultadoValidacion LogicaNegocio::validarYObtenerPlato(const QJsonObject& mensaje,
    ManejadorCliente* remitente, bool validarEstacion) {
  ResultadoValidacion resultado;

  if (!mensaje.contains(Protocolo::DATA) || !mensaje[Protocolo::DATA].isObject()) {
    qWarning() << "Mensaje sin data válida";
    return resultado;
  }

  QJsonObject data = mensaje[Protocolo::DATA].toObject();
  if (!data.contains("id_pedido") || !data.contains("id_instancia")) {
    qWarning() << "Faltan IDs en el mensaje";
    return resultado;
  }

  resultado.idPedido = data["id_pedido"].toInt();
  resultado.idInstancia = data["id_instancia"].toInt();

  auto itPedido = m_pedidosActivos.find(resultado.idPedido);
  if (itPedido == m_pedidosActivos.end()) {
    qWarning() << "Pedido no encontrado:" << resultado.idPedido;
    return resultado;
  }
  resultado.pedido = &(itPedido->second);

  for (auto& inst : resultado.pedido->platos) {
    if (inst.id_instancia == resultado.idInstancia) {
      if (validarEstacion) {
        const PlatoDefinicion& platoDef = m_menu[inst.id_plato_definicion];
        if (platoDef.estacion != remitente->getNombreEstacion().toStdString()) {
          qWarning() << "Estación" << remitente->getNombreEstacion()
                     << "no autorizada para plato de" << QString::fromStdString(platoDef.estacion);
          return resultado; // Fallo de autorización
        }
      }

      resultado.plato = &inst;
      resultado.exito = true;
      return resultado;
    }
  }

  qWarning() << "Instancia" << resultado.idInstancia << "no encontrada en pedido" << resultado.idPedido;
  return resultado;
}


