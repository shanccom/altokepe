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
#include <chrono>
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

  TipoActor tipo = cliente->getTipoActor();

  if (tipo == TipoActor::MANAGER_CHEF) {
      //estado = getEstadoParaManager(true);
  } else if (tipo == TipoActor::RANKING) {
      estado = getEstadoParaRanking();
  } else if (tipo == TipoActor::ESTACION_COCINA) {
      //estado = getEstadoParaEstacion(cliente->getNombreEstacion().toStdString());
  } else if (tipo == TipoActor::RECEPCIONISTA) {
      QJsonObject mensaje;
      QJsonArray menuArray;

      for (const auto& par : m_menu) {
          const PlatoDefinicion& plato = par.second;
          QJsonObject platoJson;
          platoJson["id"] = plato.id;
          platoJson["nombre"] = QString::fromStdString(plato.nombre);
          platoJson["precio"] = plato.costo;
          platoJson["tiempo_preparacion"] = plato.tiempo_preparacion_estimado;
          platoJson["categoria"] = QString::fromStdString(plato.estacion);
          menuArray.append(platoJson);
      }

      mensaje[Protocolo::EVENTO] = "ACTUALIZACION_MENU";
      mensaje[Protocolo::DATA] = QJsonObject{ {"menu", menuArray} };

      emit enviarRespuesta(cliente, mensaje);
      return;
  }

}

QJsonObject LogicaNegocio::getEstadoParaRanking() {
    // Nota: NO usamos lock aquí porque el llamador (enviarEstadoInicial) ya tiene el lock
    
    // 1. Convertir Mapa a Vector para ordenar
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

    // 2. Ordenar (Mayor a menor cantidad)
    std::sort(lista.begin(), lista.end(), [](const ItemRanking& a, const ItemRanking& b) {
        return a.cantidad > b.cantidad;
    });

    // 3. Construir JSON
    QJsonArray rankingArray;
    for (const auto& item : lista) {
        QJsonObject obj;
        obj["nombre"] = item.nombre;
        obj["cantidad"] = item.cantidad;
        rankingArray.append(obj);
    }

    QJsonObject mensaje;
    mensaje["evento"] = "ACTUALIZACION_RANKING";
    mensaje["data"] = QJsonObject{ {"ranking", rankingArray} };
    
    return mensaje;
}

void LogicaNegocio::registrarVenta(int idPlato) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_conteoPlatosRanking[idPlato]++;
    }

    // Notificar a todos (Observer)
    emit enviarRespuesta(nullptr, getEstadoParaRanking());
}


void LogicaNegocio::procesarMensaje(const QJsonObject& mensaje, ManejadorCliente* remitente) {
  QString comando = mensaje[Protocolo::COMANDO].toString();

  if (comando == Protocolo::NUEVO_PEDIDO) {
    procesarNuevoPedido(mensaje, remitente);

  } else if (comando == Protocolo::PREPARAR_PEDIDO) {
    procesarPrepararPedido(mensaje, remitente);

  } else if (comando == Protocolo::CANCELAR_PEDIDO) {
    procesarCancelarPedido(mensaje, remitente);

  } else if (comando == Protocolo::MARCAR_PLATO_TERMINADO) {
    procesarMarcarPlatoTerminado(mensaje, remitente);

  } else if (comando == Protocolo::CONFIRMAR_ENTREGA) {
    procesarConfirmarEntrega(mensaje, remitente);

  } else if (comando == Protocolo::DEVOLVER_PLATO) {
    //procesarDevolverPlato(mensaje, remitente);
  } else if (comando == "SOLICITAR_ESTADO") {
    //enviarEstadoInicial(remitente);  // <<=== NUEVO
  } else if (comando == "SIMULAR_VENTA") {
    // Comando de DEBUG para probar el ranking sin implementar todo el flujo de pedidos
    int idPlato = mensaje["id_plato"].toInt(1); // Por defecto plato ID 1
    qDebug() << "DEBUG: Simulando venta del plato ID:" << idPlato;
    registrarVenta(idPlato);
  } else {
    qWarning() << "Comando desconocido recibido:" << comando;
    return;
  }
}

void LogicaNegocio::cargarMenu() {
    QString ruta = "menu.json";
    QFile archivo(ruta);

    if (!archivo.open(QIODevice::ReadOnly)) {
        qCritical() << "No se pudo abrir menu.json";
        return;
    }

    QByteArray contenido = archivo.readAll();
    archivo.close();

    QJsonDocument doc = QJsonDocument::fromJson(contenido);

    if (!doc.isObject()) {
        qCritical() << "menu.json inválido";
        return;
    }

    QJsonArray arr = doc.object()["menu"].toArray();

    for (const auto& item : arr) {
        QJsonObject pjson = item.toObject();
        PlatoDefinicion p;

        p.id = pjson["id"].toInt();
        p.nombre = pjson["nombre"].toString().toStdString();
        p.estacion = pjson["categoria"].toString().toStdString();
        p.costo = pjson["precio"].toDouble();
        p.tiempo_preparacion_estimado = pjson["tiempo_preparacion"].toInt();

        m_menu[p.id] = p;
    }

    qInfo() << "Menu cargado con" << m_menu.size() << "platos.";
}

void LogicaNegocio::procesarNuevoPedido(const QJsonObject& mensaje, ManejadorCliente* remitente) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!mensaje.contains("data") || !mensaje["data"].isObject()) {
        qWarning() << "NUEVO_PEDIDO sin data";
        return;
    }

    QJsonObject data = mensaje["data"].toObject();

    if (!data.contains("platos") || !data["platos"].isArray()) {
        qWarning() << "NUEVO_PEDIDO sin platos";
        return;
    }

    long long idPedido = m_siguienteIdPedido++;

    PedidoMesa pedido;
    pedido.id = idPedido;
    pedido.estado = EstadoPedido::PENDIENTE;

    QJsonArray arr = data["platos"].toArray();

    for (auto item : arr) {
        int idPlato = item.toInt();

        if (m_menu.find(idPlato) == m_menu.end()) {
            qWarning() << "Plato inválido:" << idPlato;
            continue;
        }

        const PlatoDefinicion& def = m_menu[idPlato];

        InstanciaPlato inst;
        inst.id_instancia = m_siguienteIdInstanciaPlato++;
        inst.id_plato = def.id;
        inst.estacion = def.estacion;
        inst.nombre_plato = def.nombre;
        inst.estado = EstadoPlato::PENDIENTE;

        pedido.instancias.push_back(inst);

        InfoPlatoPrioridad pr(inst.id_instancia, def.tiempo_preparacion_estimado);
        m_colasPorEstacion[def.estacion].push(pr);
    }

    m_pedidosActivos[idPedido] = pedido;
    m_colaManagerChef.push(idPedido);

    QJsonObject msg;
    msg[Protocolo::EVENTO] = "PEDIDO_REGISTRADO";
    msg["id_pedido"] = (int)idPedido;

    emit enviarRespuesta(remitente, msg);

    for (auto cli : m_manejadoresActivos)
        if (cli->getTipoActor() == TipoActor::MANAGER_CHEF)
            emit enviarRespuesta(cli, msg);

    qInfo() << "Pedido" << idPedido << "registrado.";
}

void LogicaNegocio::procesarPrepararPedido(const QJsonObject& mensaje, ManejadorCliente* remitente) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!mensaje.contains("data") || !mensaje["data"].isObject()) {
        qWarning() << "PREPARAR_PEDIDO sin data";
        return;
    }

    QJsonObject data = mensaje["data"].toObject();

    if (!data.contains("id_instancia") || !data.contains("id_pedido")) {
        qWarning() << "PREPARAR_PEDIDO incompleto";
        return;
    }

    long long idPedido = data["id_pedido"].toInt();
    long long idInstancia = data["id_instancia"].toInt();

    if (m_pedidosActivos.find(idPedido) == m_pedidosActivos.end()) {
        qWarning() << "Pedido no encontrado:" << idPedido;
        return;
    }

    PedidoMesa& pedido = m_pedidosActivos[idPedido];

    bool encontrado = false;

    for (auto& inst : pedido.instancias) {
        if (inst.id_instancia == idInstancia) {

            if (inst.estacion != remitente->getNombreEstacion().toStdString()) {
                qWarning() << "Estación no autorizada para preparar plato.";
                return;
            }

            inst.estado = EstadoPlato::EN_PREPARACION;
            encontrado = true;
            break;
        }
    }

    if (!encontrado) {
        qWarning() << "Instancia no encontrada en pedido.";
        return;
    }

    QJsonObject msg;
    msg[Protocolo::EVENTO] = "PLATO_EN_PREPARACION";
    msg["id_pedido"] = (int)idPedido;
    msg["id_instancia"] = (int)idInstancia;

    for (auto cli : m_manejadoresActivos)
        emit enviarRespuesta(cli, msg);

    qInfo() << "Plato" << idInstancia << "del pedido" << idPedido << "pasó a EN_PREPARACION.";
}

void LogicaNegocio::procesarCancelarPedido(const QJsonObject& mensaje, ManejadorCliente* remitente) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!mensaje.contains("data") || !mensaje["data"].isObject()) {
        qWarning() << "CANCELAR_PEDIDO sin data";
        return;
    }

    QJsonObject data = mensaje["data"].toObject();

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

    pedido.estado = EstadoPedido::CANCELADO;

    for (auto& inst : pedido.instancias) {
        inst.estado = EstadoPlato::CANCELADO;
    }

    QJsonObject msg;
    msg[Protocolo::EVENTO] = "PEDIDO_CANCELADO";
    msg["id_pedido"] = (int)idPedido;

    // Notifica a todos los roles
    for (auto cli : m_manejadoresActivos)
        emit enviarRespuesta(cli, msg);

    qInfo() << "Pedido" << idPedido << "ha sido CANCELADO.";
}

void LogicaNegocio::procesarMarcarPlatoTerminado(const QJsonObject& mensaje, ManejadorCliente* remitente) {

    std::lock_guard<std::mutex> lock(m_mutex);

    if (!mensaje.contains("data") || !mensaje["data"].isObject()) {
        qWarning() << "MARCAR_PLATO_TERMINADO sin data";
        return;
    }

    QJsonObject data = mensaje["data"].toObject();

    if (!data.contains("id_pedido") || !data.contains("id_instancia")) {
        qWarning() << "Faltan datos en MARCAR_PLATO_TERMINADO";
        return;
    }

    long long idPedido = data["id_pedido"].toInt();
    long long idInstancia = data["id_instancia"].toInt();

    if (m_pedidosActivos.find(idPedido) == m_pedidosActivos.end()) {
        qWarning() << "Pedido no encontrado:" << idPedido;
        return;
    }

    PedidoMesa& pedido = m_pedidosActivos[idPedido];

    bool encontrado = false;

    for (auto& inst : pedido.instancias) {

        if (inst.id_instancia == idInstancia) {

            // ✔ Validar estación
            if (inst.estacion != remitente->getNombreEstacion().toStdString()) {
                qWarning() << "Estación no autorizada para marcar plato terminado";
                return;
            }

            inst.estado = EstadoPlato::TERMINADO;
            encontrado = true;
            break;
        }
    }

    if (!encontrado) {
        qWarning() << "Instancia no encontrada:" << idInstancia;
        return;
    }

    bool todoTerminado = true;

    for (const auto& inst : pedido.instancias) {
        if (inst.estado != EstadoPlato::TERMINADO) {
            todoTerminado = false;
            break;
        }
    }

    if (todoTerminado) {
        pedido.estado = EstadoPedido::LISTO;
    }

    QJsonObject msg;
    msg[Protocolo::EVENTO] = "PLATO_TERMINADO";
    msg["id_pedido"] = (int)idPedido;
    msg["id_instancia"] = (int)idInstancia;
    msg["pedido_listo"] = todoTerminado;

    for (auto cli : m_manejadoresActivos)
        emit enviarRespuesta(cli, msg);

    qInfo() << "Plato" << idInstancia << "terminado. Pedido" 
            << idPedido << (todoTerminado ? "LISTO" : "AÚN EN PROCESO");
}

void LogicaNegocio::procesarConfirmarEntrega(const QJsonObject& mensaje, ManejadorCliente* remitente) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!mensaje.contains("data") || !mensaje["data"].isObject()) {
        qWarning() << "CONFIRMAR_ENTREGA sin data";
        return;
    }

    QJsonObject data = mensaje["data"].toObject();

    if (!data.contains("id_pedido")) {
        qWarning() << "CONFIRMAR_ENTREGA sin id_pedido";
        return;
    }

    long long idPedido = data["id_pedido"].toInt();

    auto it = m_pedidosActivos.find(idPedido);
    if (it == m_pedidosActivos.end()) {
        qWarning() << "Pedido no encontrado en CONFIRMAR_ENTREGA:" << idPedido;
        return;
    }

    PedidoMesa& pedido = it->second;

    if (pedido.estado != EstadoPedido::LISTO) {
        qWarning() << "No se puede confirmar entrega: pedido no está LISTO. Estado actual:" << (int)pedido.estado;
        return;
    }

    pedido.estado = EstadoPedido::ENTREGADO;

    for (auto& inst : pedido.instancias) {
        inst.estado = EstadoPlato::ENTREGADO;
    }

    QJsonObject msg;
    msg[Protocolo::EVENTO] = "PEDIDO_ENTREGADO";
    msg["id_pedido"] = (int)idPedido;

    for (auto cli : m_manejadoresActivos) {
        emit enviarRespuesta(cli, msg);
    }

    for (const auto& inst : pedido.instancias) {
        m_conteoPlatosRanking[inst.id_plato]++;
    }

    qInfo() << "Pedido" << idPedido << "ENTREGADO correctamente.";
}

void LogicaNegocio::procesarDevolverPlato(const QJsonObject& mensaje, ManejadorCliente* remitente)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!mensaje.contains("data") || !mensaje["data"].isObject()) {
        qWarning() << "DEVOLVER_PLATO sin data";
        return;
    }

    QJsonObject data = mensaje["data"].toObject();

    if (!data.contains("id_pedido") || !data.contains("id_instancia")) {
        qWarning() << "DEVOLVER_PLATO incompleto";
        return;
    }

    long long idPedido = data["id_pedido"].toInt();
    long long idInstancia = data["id_instancia"].toInt();

    auto it = m_pedidosActivos.find(idPedido);
    if (it == m_pedidosActivos.end()) {
        qWarning() << "Pedido no encontrado en DEVOLVER_PLATO:" << idPedido;
        return;
    }

    PedidoMesa& pedido = it->second;

    bool encontrado = false;
    std::string estacionObjetivo;

    for (auto& inst : pedido.instancias) {
        if (inst.id_instancia == idInstancia) {

            if (inst.estado != EstadoPlato::ENTREGADO) {
                qWarning() << "No se puede devolver un plato que NO está ENTREGADO. Estado actual:"
                           << (int)inst.estado;
                return;
            }

            inst.estado = EstadoPlato::DEVUELTO;
            estacionObjetivo = inst.estacion;
            encontrado = true;
            break;
        }
    }

    if (!encontrado) {
        qWarning() << "Instancia no encontrada en pedido en DEVOLVER_PLATO. ID:" << idInstancia;
        return;
    }

    pedido.estado = EstadoPedido::EN_PREPARACION;

    const PlatoDefinicion& def = m_menu[pedido.instancias[0].id_plato];

    InfoPlatoPrioridad pr(idInstancia, def.tiempo_preparacion_estimado);
    m_colasPorEstacion[estacionObjetivo].push(pr);

    for (const auto& inst : pedido.instancias) {
        if (inst.id_instancia == idInstancia) {
            m_conteoPlatosRanking[inst.id_plato]--;
            break;
        }
    }

    QJsonObject msg;
    msg[Protocolo::EVENTO] = "PLATO_DEVUELTO";
    msg["id_pedido"] = (int)idPedido;
    msg["id_instancia"] = (int)idInstancia;
    msg["estacion"] = QString::fromStdString(estacionObjetivo);

    for (auto cli : m_manejadoresActivos)
        emit enviarRespuesta(cli, msg);

    qInfo() << "Plato" << idInstancia << "del pedido" << idPedido << "ha sido DEVUELTO.";
}

