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
#include <set>

LogicaNegocio* LogicaNegocio::s_instance = nullptr;

LogicaNegocio::LogicaNegocio(QObject* parent) 
  : QObject(parent), 
    m_serializador(),
    m_menuRepository(m_serializador),
    m_pedidoRepository()
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
  m_menuRepository.cargarDesdeArchivo(rutaArchivo);
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
    estado = construirEstadoManagerChef();
    emit enviarRespuesta(cliente, estado);

  } else if (tipo == TipoActor::RANKING) {
    estado = getEstadoParaRanking();
    emit enviarRespuesta(cliente, estado);

  } else if (tipo == TipoActor::ESTACION_COCINA) {
    // estado = getEstadoParaEstacion(cliente->getNombreEstacion().toStdString());
  
  } else if (tipo == TipoActor::RECEPCIONISTA) {
    QJsonObject mensaje;
    QJsonArray menuArray;

    for (const auto& par : m_menuRepository.menu()) {
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

  auto conteo = m_pedidoRepository.obtenerConteoRanking();

  for (auto const& [id, cantidad] : conteo) {
    const PlatoDefinicion* def = m_menuRepository.obtenerPlato(id);
    if (def) {
      lista.push_back({QString::fromStdString(def->nombre), cantidad});
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
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pedidoRepository.incrementarConteoRanking(idPlato);
  }
  notificarActualizacionRanking();
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
  
  long long idPedido = m_pedidoRepository.generarNuevoIdPedido();

  PedidoMesa pedido;
  pedido.id_pedido = idPedido;
  pedido.numero_mesa = data["mesa"].toInt();
  pedido.id_recepcionista = data["id_recepcionista"].toInt();
  pedido.estado_general = EstadoPedido::PENDIENTE;

  QJsonArray platosArray = data["platos"].toArray();
  const auto& menu = m_menuRepository.menu();  

  for (const QJsonValue& valor : platosArray) {
    QJsonObject platoObj = valor.toObject();

    // Extraemos campos específicos del objeto enviado por PanelPedido
    int idPlato = platoObj["id"].toInt();
    int cantidad = platoObj["cantidad"].toInt();

    if (menu.find(idPlato) == menu.end()) {
      qWarning() << "Plato inválido:" << idPlato;
      continue;
    }
    if (cantidad <= 0) {
      qWarning() << "Cantidad inválida para plato:" << idPlato;
      continue;
    }

    const PlatoDefinicion& platoDef = menu.at(idPlato);

    for (int i = 0; i < cantidad; ++i) {
      PlatoInstancia platoInst;
      platoInst.id_instancia = m_pedidoRepository.generarNuevoIdInstancia();
      platoInst.id_plato_definicion = platoDef.id;
      platoInst.estado = EstadoPlato::EN_ESPERA;

      pedido.platos.push_back(platoInst);
    }
  }

  if (pedido.platos.empty()) {
    qWarning() << "Pedido" << idPedido << "ignorado: no contiene platos válidos.";
    return;
  }

  m_pedidoRepository.pedidos()[idPedido] = pedido; //Se guarda en el repositorio
  m_pedidoRepository.colaManagerChef().push(idPedido);//se inserta en managerchef

  QJsonObject pedidoJson = m_serializador.pedidoMesaToJson(pedido);

  QJsonObject msg;
  msg[Protocolo::EVENTO] = Protocolo::PEDIDO_REGISTRADO;
  msg[Protocolo::DATA] = pedidoJson;

  emit enviarRespuesta(remitente, msg);

  for (auto cli : m_manejadoresActivos)
    if (cli->getTipoActor() == TipoActor::MANAGER_CHEF) emit enviarRespuesta(cli, msg);

  qInfo() << "Pedido" << idPedido << "registrado para Mesa" << pedido.numero_mesa << "con" << pedido.platos.size() << "items.";
}

void LogicaNegocio::procesarPrepararPedido(const QJsonObject& mensaje, ManejadorCliente* remitente) {
  std::lock_guard<std::mutex> lock(m_mutex);

  if (!mensaje.contains(Protocolo::DATA) || !mensaje[Protocolo::DATA].isObject()) {
    enviarError(remitente, "Payload inválido en PREPARAR_PEDIDO");
    return;
  }
  QJsonObject data = mensaje[Protocolo::DATA].toObject();

  if (!data.contains("id_pedido")) {
    enviarError(remitente, "Falta id_pedido en PREPARAR_PEDIDO");
    return;
  }
  long long idPedido = data["id_pedido"].toInt();

  auto& colaChef = m_pedidoRepository.colaManagerChef();
  auto& pedidos  = m_pedidoRepository.pedidos();
  const auto& menu = m_menuRepository.menu();
  auto& colasEstacion = m_pedidoRepository.colasPorEstacion();

  // Validación Estricta de Orden (FIFO)
  if (colaChef.empty()) {
    enviarError(remitente, "No hay pedidos en cola para preparar.", data);
    return;
  }

  // Verificamos si el pedido solicitado es el que está en el frente de la cola
  if (colaChef.front() != idPedido) {
    enviarError(remitente, "Solo se puede preparar el pedido que está al inicio de la cola.", data);
    return;
  }

  if (pedidos.find(idPedido) == pedidos.end()) {
    enviarError(remitente, "Pedido no encontrado para preparar", data);
    // Si no está en el mapa pero sí en la cola (estado inconsistente), lo sacamos de la cola
    colaChef.pop();
    return;
  }

  PedidoMesa& pedido = pedidos[idPedido];
  colaChef.pop();

  if (pedido.estado_general == EstadoPedido::PENDIENTE) {
    pedido.estado_general = EstadoPedido::EN_PROGRESO;
  }

  int platosIniciados = 0;
  for (auto& inst : pedido.platos) {
    if (inst.estado == EstadoPlato::EN_ESPERA) {
      const PlatoDefinicion& platoDef = menu.at(inst.id_plato_definicion);

      inst.estado = EstadoPlato::EN_PROGRESO;

      InfoPlatoPrioridad platoPrior(inst.id_instancia, platoDef.tiempo_preparacion_estimado);
      platoPrior.id_pedido = idPedido;
      colasEstacion[platoDef.estacion].push(platoPrior);

      platosIniciados++;

      QJsonObject dataResp;
      dataResp["id_pedido"] = static_cast<int>(idPedido);
      dataResp["id_instancia"] = static_cast<int>(inst.id_instancia);
      dataResp["nombre"] = QString::fromStdString(platoDef.nombre);
      dataResp["score"] = platoDef.tiempo_preparacion_estimado; // O lógica de prioridad
      dataResp["nuevo_estado"] = SerializadorJSON::estadoPlatoToString(EstadoPlato::EN_PROGRESO);
      dataResp["estacion"] = QString::fromStdString(platoDef.estacion);

      QJsonObject msg;
      msg[Protocolo::EVENTO] = Protocolo::NUEVO_PLATO_EN_COLA;
      msg[Protocolo::DATA] = dataResp;

      QJsonObject msgPedido;
      msgPedido[Protocolo::EVENTO] = Protocolo::PLATO_EN_PREPARACION;
      msgPedido[Protocolo::DATA] = dataResp;

      for (auto cli : m_manejadoresActivos) {
        if (cli->getTipoActor() == TipoActor::MANAGER_CHEF) emit enviarRespuesta(cli, msgPedido);
        else emit enviarRespuesta(cli, msg);
      }
    }
  }

  // Actualización de TOP de Colas
  for (auto& [nombreEstacion, colaPrioridad] : colasEstacion) {
    if (colaPrioridad.empty()) continue;

    const InfoPlatoPrioridad& topInfo = colaPrioridad.top();

  if (pedidos.find(topInfo.id_pedido) != pedidos.end()) {
      PedidoMesa& pedidoTop = pedidos[topInfo.id_pedido];

      for (auto& inst : pedidoTop.platos) {
        if (inst.id_instancia == topInfo.id_instancia_plato && inst.estado == EstadoPlato::EN_PROGRESO) {
          // Solo actualizamos si no estaba ya cocinándose
          inst.estado = EstadoPlato::PREPARANDO;

          QJsonObject dataTop;
          dataTop["id_pedido"] = static_cast<int>(topInfo.id_pedido);
          dataTop["id_instancia"] = static_cast<int>(inst.id_instancia);
          dataTop["nuevo_estado"] = SerializadorJSON::estadoPlatoToString(EstadoPlato::PREPARANDO);

          QJsonObject msgTop;
          msgTop[Protocolo::EVENTO] = Protocolo::PLATO_ESTADO_CAMBIADO;
          msgTop[Protocolo::DATA] = dataTop;

          QJsonObject msgPedido;
          msgPedido[Protocolo::EVENTO] = Protocolo::PLATO_EN_PREPARACION;
          msgPedido[Protocolo::DATA] = dataTop;

          for (auto cli : m_manejadoresActivos) {
            if (cli->getTipoActor() == TipoActor::MANAGER_CHEF) emit enviarRespuesta(cli, msgPedido);
            else emit enviarRespuesta(cli, msgTop);
          }

          qInfo() << "Plato" << inst.id_instancia << "de estación"
                  << QString::fromStdString(nombreEstacion) << "ahora está PREPARANDO."; 
        }
      }
    }
  }

  if (platosIniciados > 0) qInfo() << "Se iniciaron" << platosIniciados << "platos del pedido" << idPedido;
  else qInfo() << "Solicitud de preparar pedido" << idPedido << "recibida, pero no había platos en espera.";
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

  // Consulta al repositorio en lugar del mapa directo
  auto& pedidosRepo = m_pedidoRepository.pedidos();

  if (pedidosRepo.find(idPedido) == pedidosRepo.end()) {
    enviarError(remitente, "El pedido no existe.", data);
    return;
  }

  PedidoMesa& pedido = pedidosRepo[idPedido];

  // Validad el estado micro de los platos
  bool cocinaTrabajando = false;
  for (const auto& inst : pedido.platos) {
    if (inst.estado == EstadoPlato::PREPARANDO ||
      inst.estado == EstadoPlato::FINALIZADO || 
      inst.estado == EstadoPlato::ENTREGADO) {
      cocinaTrabajando = true;
      break;
    }
  }

  if (cocinaTrabajando) {
    enviarError(remitente, "No se puede cancelar: La cocina ya está preparando este pedido.", data);
    return;
  }

  if (pedido.estado_general == EstadoPedido::ENTREGADO || 
      pedido.estado_general == EstadoPedido::CANCELADO) {
    qWarning() << "Intento de cancelar pedido en estado final:" << idPedido;
    return;
  }

  pedido.estado_general = EstadoPedido::CANCELADO;
  for (auto& inst : pedido.platos) inst.estado = EstadoPlato::CANCELADO;

  QJsonObject msg;
  msg[Protocolo::EVENTO] = Protocolo::PEDIDO_CANCELADO;
  msg[Protocolo::DATA] = QJsonObject{ {"id_pedido", static_cast<int>(idPedido)} };

  // Notifica a todos los roles
  for (auto cli : m_manejadoresActivos) emit enviarRespuesta(cli, msg);

  qInfo() << "Pedido" << idPedido << "ha sido marcado como CANCELADO.";
}

void LogicaNegocio::procesarMarcarPlatoTerminado(const QJsonObject& mensaje, ManejadorCliente* remitente) {
  std::lock_guard<std::mutex> lock(m_mutex);

  if (!mensaje.contains(Protocolo::DATA) || !mensaje[Protocolo::DATA].isObject()) {
    enviarError(remitente, "Mensaje sin data válida");
    return;
  }
  const QJsonObject data = mensaje[Protocolo::DATA].toObject();

  if (!data.contains("id_pedido") || !data.contains("id_instancia")) {
    enviarError(remitente, "Faltan IDs (pedido o instancia) en el mensaje", data);
    return;
  }

  const int idPedido = data["id_pedido"].toInt();
  const int idInstancia = data["id_instancia"].toInt();

  auto& pedidosRepo = m_pedidoRepository.pedidos();

  auto itPedido = pedidosRepo.find(idPedido);
  if (itPedido == pedidosRepo.end()) {
    enviarError(remitente, "Pedido no encontrado", data);
    return;
  }

  PedidoMesa& pedido = itPedido->second;
  PlatoInstancia* instancia = nullptr;
  std::string nombreEstacion;

  for (auto& inst : pedido.platos) {
    if (inst.id_instancia == idInstancia) {

      const PlatoDefinicion* def = m_menuRepository.obtenerPlato(inst.id_plato_definicion);
      if (!def) {
        enviarError(remitente, "Definición de plato no encontrada", data);
        return;
      }

      nombreEstacion = def->estacion;

      if (def->estacion != remitente->getNombreEstacion().toStdString()) {
        qWarning() << "Estación" << remitente->getNombreEstacion()
                   << "no autorizada para plato de"
                   << QString::fromStdString(def->estacion);
        return;
      }

      instancia = &inst;
      break;
    }
  }

  if (!instancia) {
    enviarError(remitente, "Instancia de plato no encontrada en el pedido especificado", data);
    return;
  }

  instancia->estado = EstadoPlato::FINALIZADO;
  m_pedidoRepository.incrementarConteoRanking(instancia->id_plato_definicion);

  // Actualiza cola de estación 
  if (!nombreEstacion.empty()) {
    auto& cola = m_pedidoRepository.colasPorEstacion()[nombreEstacion]; 
    if (cola.empty()) {
      qWarning() << "La cola de estación" << QString::fromStdString(nombreEstacion) << "ya estaba vacía.";
      return;
    }

    if (cola.top().id_instancia_plato != idInstancia) {
      enviarError(remitente, "Solo se puede finalizar el plato que se está PREPARANDO.", data);
      return;
    }

    cola.pop();

    // Se actualiza el Top
    if (!cola.empty()) {
      const InfoPlatoPrioridad& nuevoTop = cola.top();

      if (pedidosRepo.find(nuevoTop.id_pedido) != pedidosRepo.end()) {
        PedidoMesa& pedidoTop = pedidosRepo[nuevoTop.id_pedido];

        for (auto& instTop : pedidoTop.platos) {
          if (instTop.id_instancia == nuevoTop.id_instancia_plato) {

            if (instTop.estado == EstadoPlato::EN_PROGRESO ||
                instTop.estado == EstadoPlato::DEVUELTO)
            {
              instTop.estado = EstadoPlato::PREPARANDO;

              QJsonObject dataTop;
              dataTop["id_pedido"] = static_cast<int>(nuevoTop.id_pedido);
              dataTop["id_instancia"] = static_cast<int>(instTop.id_instancia);
              dataTop["nuevo_estado"] = SerializadorJSON::estadoPlatoToString(EstadoPlato::PREPARANDO);

              QJsonObject msgTop;
              msgTop[Protocolo::EVENTO] = Protocolo::PLATO_ESTADO_CAMBIADO;
              msgTop[Protocolo::DATA] = dataTop;

              QJsonObject msgPedido;
              msgPedido[Protocolo::EVENTO] = Protocolo::PLATO_EN_PREPARACION;
              msgPedido[Protocolo::DATA] = dataTop;

              for (auto cli : m_manejadoresActivos) {
                if (cli->getTipoActor() == TipoActor::MANAGER_CHEF)
                  emit enviarRespuesta(cli, msgPedido);
                else
                  emit enviarRespuesta(cli, msgTop);
              }

              qInfo() << "Estación" << QString::fromStdString(nombreEstacion)
                      << ": Nuevo plato PREPARANDO (ID: " << instTop.id_instancia << ")";
            }
            break;
          }
        }
      }
    }
  }

  bool todoTerminado = true;
  for (const auto& inst : pedido.platos) {
    if (inst.estado != EstadoPlato::FINALIZADO &&
        inst.estado != EstadoPlato::CANCELADO) 
    {
      todoTerminado = false;
      break;
    }
  }

  if (todoTerminado)
    pedido.estado_general = EstadoPedido::LISTO;

  QJsonObject dataEnvio;
  dataEnvio["id_pedido"] = static_cast<int>(idPedido);
  dataEnvio["id_instancia"] = static_cast<int>(idInstancia);
  dataEnvio["pedido_listo"] = todoTerminado;
  dataEnvio["nuevo_estado"] = SerializadorJSON::estadoPlatoToString(EstadoPlato::FINALIZADO);

  QJsonObject msg;
  msg[Protocolo::EVENTO] = Protocolo::PLATO_TERMINADO;
  msg[Protocolo::DATA] = dataEnvio;

  for (auto cli : m_manejadoresActivos) emit enviarRespuesta(cli, msg);

  notificarActualizacionRanking();
  qInfo() << "Plato" << idInstancia << "terminado. Pedido"
          << idPedido << (todoTerminado ? "LISTO" : "AÚN EN PROCESO");
}

void LogicaNegocio::procesarDevolverPlato(const QJsonObject& mensaje, ManejadorCliente* remitente) {
  std::lock_guard<std::mutex> lock(m_mutex);

  if (!mensaje.contains(Protocolo::DATA) || !mensaje[Protocolo::DATA].isObject()) {
    enviarError(remitente, "Payload inválido en DEVOLVER_PLATO");
    return;
  }
  QJsonObject data = mensaje[Protocolo::DATA].toObject();

  if (!data.contains("id_pedido")) {
    enviarError(remitente, "Falta id_pedido para devolver plato");
    return;
  }
  long long idPedido = data["id_pedido"].toInt();

  auto& pedidosRepo = m_pedidoRepository.pedidos();

  if (pedidosRepo.find(idPedido) == pedidosRepo.end()) {
    enviarError(remitente, "Pedido no existe", data);
    return;
  }

  PedidoMesa& pedido = pedidosRepo[idPedido];
  std::vector<PlatoInstancia*> platosADevolver;

  // Lógica dual (id_instancia específico O pedido completo)
  if (data.contains("id_instancia")) {
    long long idInstancia = data["id_instancia"].toInt();
    bool encontrado = false;
    for (auto& inst : pedido.platos) {
      if (inst.id_instancia == idInstancia) {
        platosADevolver.push_back(&inst);
        encontrado = true;
        break;
      }
    }
    if (!encontrado) {
      enviarError(remitente, "Instancia de plato no encontrada en el pedido", data);
      return;
    }
  } else {
    for (auto& inst : pedido.platos) {
      if (inst.estado == EstadoPlato::ENTREGADO || inst.estado == EstadoPlato::FINALIZADO) {
        platosADevolver.push_back(&inst);
      }
    }
    if (platosADevolver.empty()) {
      enviarError(remitente, "No hay platos elegibles para devolver en este pedido", data);
      return;
    }
  }

  pedido.estado_general = EstadoPedido::EN_PROGRESO;
  int platosDevueltosCount = 0;
  std::set<std::string> estacionesAfectadas;

  for (auto* plato : platosADevolver) {
    if (plato->estado == EstadoPlato::DEVUELTO) continue;

    const PlatoDefinicion* def = m_menuRepository.obtenerPlato(plato->id_plato_definicion);
    if (!def) {
      qWarning() << "Definición de plato no encontrada";
      continue;
    }

    double nuevoScore = def->tiempo_preparacion_estimado * 1.5;
    std::string estacionObjetivo = def->estacion;
    estacionesAfectadas.insert(estacionObjetivo);

    plato->estado = EstadoPlato::DEVUELTO;

    InfoPlatoPrioridad platoPrior(plato->id_instancia, nuevoScore);
    platoPrior.id_pedido = idPedido;
    m_pedidoRepository.colasPorEstacion()[estacionObjetivo].push(platoPrior);

    m_pedidoRepository.decrementarConteoRanking(plato->id_plato_definicion);

    QJsonObject dataResp;
    dataResp["id_pedido"] = static_cast<int>(idPedido);
    dataResp["id_instancia"] = static_cast<int>(plato->id_instancia);
    dataResp["nombre"] = QString::fromStdString(def->nombre);
    dataResp["estacion"] = QString::fromStdString(estacionObjetivo);
    dataResp["score"] = nuevoScore;
    dataResp["nuevo_estado"] = SerializadorJSON::estadoPlatoToString(EstadoPlato::DEVUELTO);

    QJsonObject msg;
    msg[Protocolo::EVENTO] = Protocolo::PLATO_DEVUELTO;
    msg[Protocolo::DATA] = dataResp;

    for (auto cli : m_manejadoresActivos) emit enviarRespuesta(cli, msg);
    platosDevueltosCount++;
  }

  // Al insertar con prioridad alta, es probable que el devuelto sea el nuevo Top.
  for (const std::string& nombreEstacion : estacionesAfectadas) {
    auto& cola = m_pedidoRepository.colasPorEstacion()[nombreEstacion];
    if (cola.empty()) continue;

    const InfoPlatoPrioridad& topInfo = cola.top();

    if (pedidosRepo.find(topInfo.id_pedido) != pedidosRepo.end()) {
      PedidoMesa& pedidoTop = pedidosRepo[topInfo.id_pedido];
      for (auto& instTop : pedidoTop.platos) {
        if (instTop.id_instancia == topInfo.id_instancia_plato) {
          if (instTop.estado == EstadoPlato::DEVUELTO || instTop.estado == EstadoPlato::EN_PROGRESO) {
            instTop.estado = EstadoPlato::PREPARANDO;

            QJsonObject dataTop;
            dataTop["id_pedido"] = static_cast<int>(topInfo.id_pedido);
            dataTop["id_instancia"] = static_cast<int>(instTop.id_instancia);
            dataTop["nuevo_estado"] = SerializadorJSON::estadoPlatoToString(EstadoPlato::PREPARANDO);

            QJsonObject msgTop;
            msgTop[Protocolo::EVENTO] = Protocolo::PLATO_ESTADO_CAMBIADO;
            msgTop[Protocolo::DATA] = dataTop;

            QJsonObject msgPedido;
            msgPedido[Protocolo::EVENTO] = Protocolo::PLATO_EN_PREPARACION;
            msgPedido[Protocolo::DATA] = dataTop;

            for (auto cli : m_manejadoresActivos) {
              if (cli->getTipoActor() == TipoActor::MANAGER_CHEF)
                emit enviarRespuesta(cli, msgPedido);
              else
                emit enviarRespuesta(cli, msgTop);
            }

            qInfo() << "Estación" << QString::fromStdString(nombreEstacion)
                    << ": Plato devuelto ahora está PREPARANDO (ID:" << instTop.id_instancia << ")";
          }
          break;
        }
      }
    }
  }

  qInfo() << "Se devolvieron" << platosDevueltosCount << "del pedido" << idPedido;

  if (platosDevueltosCount > 0)
    notificarActualizacionRanking();
}

void LogicaNegocio::procesarConfirmarEntrega(const QJsonObject& mensaje, ManejadorCliente* remitente) {
  std::lock_guard<std::mutex> lock(m_mutex);

  if (!mensaje.contains(Protocolo::DATA) || !mensaje[Protocolo::DATA].isObject()) {
    enviarError(remitente, "Payload inválido en CONFIRMAR_ENTREGA"); 
    return;
  }
  QJsonObject data = mensaje[Protocolo::DATA].toObject();

  if (!data.contains("id_pedido")) {
    enviarError(remitente, "Falta id_pedido en CONFIRMAR_ENTREGA");
    return;
  }
  long long idPedido = data["id_pedido"].toInt();

  auto& pedidosRepo = m_pedidoRepository.pedidos();

  if (pedidosRepo.find(idPedido) == pedidosRepo.end()) {
    enviarError(remitente, "Pedido no encontrado", data);
    return;
  }

  PedidoMesa& pedido = pedidosRepo[idPedido];

  if (pedido.estado_general != EstadoPedido::LISTO) {
    enviarError(remitente, "El pedido no está LISTO para entrega", data);
    return;
  }

  pedido.estado_general = EstadoPedido::ENTREGADO;
  for (auto& inst : pedido.platos) inst.estado = EstadoPlato::ENTREGADO;

  QJsonObject msg;
  msg[Protocolo::EVENTO] = Protocolo::PEDIDO_ENTREGADO;
  msg[Protocolo::DATA] = QJsonObject{ {"id_pedido", static_cast<int>(idPedido)} };

  for (auto cli : m_manejadoresActivos) emit enviarRespuesta(cli, msg);

  for (const auto& inst : pedido.platos) {
    m_pedidoRepository.incrementarConteoRanking(inst.id_plato_definicion);
  }

  notificarActualizacionRanking();

  qInfo() << "Pedido" << idPedido << "ENTREGADO correctamente.";
}

QJsonObject LogicaNegocio::construirEstadoManagerChef() {
  QJsonArray menuArray;
  for (const auto& par : m_menuRepository.menu()) {
    menuArray.append(SerializadorJSON::platoDefinicionToJson(par.second));
  }

  QJsonArray pendientes;
  QJsonArray enProgreso;
  QJsonArray terminados;

  auto& pedidosRepo = m_pedidoRepository.pedidos();

  for (const auto& par : pedidosRepo) {
    const PedidoMesa& pedido = par.second;
    QJsonObject pedidoJson = SerializadorJSON::pedidoMesaToJson(pedido);

    switch (pedido.estado_general) {
      case EstadoPedido::PENDIENTE:
        pendientes.append(pedidoJson);
        break;

      case EstadoPedido::EN_PROGRESO:
        enProgreso.append(pedidoJson);
        break;

      case EstadoPedido::LISTO:
      case EstadoPedido::ENTREGADO:
      case EstadoPedido::CANCELADO:
        terminados.append(pedidoJson);
        break;

      default:
        break;
    }
  }

  QJsonObject data;
  data["menu"] = menuArray;
  data["pedidos_pendientes"] = pendientes;
  data["pedidos_en_progreso"] = enProgreso;
  data["pedidos_terminados"] = terminados;

  QJsonObject mensaje;
  mensaje[Protocolo::EVENTO] = Protocolo::ACTUALIZACION_ESTADO_GENERAL;
  mensaje[Protocolo::DATA] = data;

  return mensaje;
}


void LogicaNegocio::enviarError(ManejadorCliente* cliente, const QString& mensajeError, const QJsonObject& dataContexto) {
  QJsonObject msg;
  msg[Protocolo::EVENTO] = Protocolo::ERROR;
  msg[Protocolo::MENSAJE_ERROR] = mensajeError;
  if (!dataContexto.isEmpty()) {
    msg[Protocolo::DATA] = dataContexto;
  }

  if (cliente) emit enviarRespuesta(cliente, msg);

  qWarning() << "Error enviado al cliente:" << mensajeError;
}

void LogicaNegocio::notificarActualizacionRanking() {
  QJsonObject rankingMsg = getEstadoParaRanking();
  // nullptr indica broadcast (el Servidor maneja el filtro para enviar solo a pantallas Ranking)
  emit enviarRespuesta(nullptr, rankingMsg);
}