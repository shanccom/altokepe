#include "SerializadorJSON.h"
#include <QJsonArray>
#include <QDebug>
#include <QJsonArray>
#include <QDebug>

QJsonObject SerializadorJSON::platoDefinicionToJson(const PlatoDefinicion& plato) {
  QJsonObject json;
  json["id"] = plato.id;
  json["nombre"] = QString::fromStdString(plato.nombre);
  json["costo"] = plato.costo;
  json["tiempo_preparacion_estimado"] = plato.tiempo_preparacion_estimado;
  json["estacion"] = QString::fromStdString(plato.estacion);
  return json;
}

PlatoDefinicion SerializadorJSON::jsonToPlatoDefinicion(const QJsonObject& json) {
  if (!json.contains("id")) throw ExcepcionCampoFaltante("id");
  if (!json.contains("nombre")) throw ExcepcionCampoFaltante("nombre");
  if (!json.contains("costo")) throw ExcepcionCampoFaltante("costo");
  if (!json.contains("tiempo_preparacion_estimado")) throw ExcepcionCampoFaltante("tiempo_preparacion_estimado");
  if (!json.contains("estacion")) throw ExcepcionCampoFaltante("estacion");

  if (!json["id"].isDouble()) throw ExcepcionTipoIncorrecto("id", "int", "otro tipo");
  if (!json["nombre"].isString()) throw ExcepcionTipoIncorrecto("nombre", "string", "otro tipo");
  if (!json["costo"].isDouble()) throw ExcepcionTipoIncorrecto("costo", "double", "otro tipo");
  if (!json["tiempo_preparacion_estimado"].isDouble()) throw ExcepcionTipoIncorrecto("tiempo_preparacion_estimado", "int", "otro tipo");
  if (!json["estacion"].isString()) throw ExcepcionTipoIncorrecto("estacion", "string", "otro tipo");

  PlatoDefinicion plato;
  plato.id = json["id"].toInt();
  plato.nombre = json["nombre"].toString().toStdString();
  plato.costo = json["costo"].toDouble();
  plato.tiempo_preparacion_estimado = json["tiempo_preparacion_estimado"].toInt();
  plato.estacion = json["estacion"].toString().toStdString();

  return plato;
}

QJsonObject SerializadorJSON::platoInstanciaToJson(const PlatoInstancia& plato) {
  QJsonObject json;
  json["id_instancia"] = QJsonValue::fromVariant(QVariant::fromValue(plato.id_instancia));
  json["id_plato_definicion"] = plato.id_plato_definicion;
  json["estado"] = estadoPlatoToString(plato.estado);
  return json;
}

PlatoInstancia SerializadorJSON::jsonToPlatoInstancia(const QJsonObject& json) {
  if (!json.contains("id_instancia")) throw ExcepcionCampoFaltante("id_instancia");
  if (!json.contains("id_plato_definicion")) throw ExcepcionCampoFaltante("id_plato_definicion");
  if (!json.contains("estado")) throw ExcepcionCampoFaltante("estado");

  if (!json["id_instancia"].isDouble() && !json["id_instancia"].isString())
      throw ExcepcionTipoIncorrecto("id_instancia", "long long", "otro tipo");

  if (!json["id_plato_definicion"].isDouble())
      throw ExcepcionTipoIncorrecto("id_plato_definicion", "int", "otro tipo");

  if (!json["estado"].isString())
      throw ExcepcionTipoIncorrecto("estado", "string", "otro tipo");

  PlatoInstancia plato;
  plato.id_instancia = json["id_instancia"].toVariant().toLongLong();
  plato.id_plato_definicion = json["id_plato_definicion"].toInt();
  plato.estado = stringToEstadoPlato(json["estado"].toString());

  return plato;
}

QJsonObject SerializadorJSON::pedidoMesaToJson(const PedidoMesa& pedido) {
    // Validaciones básicas antes de serializar
    if (pedido.id_pedido <= 0) {
        throw ExcepcionJSON("El campo 'id_pedido' es inválido (<= 0)");
    }

    if (pedido.numero_mesa <= 0) {
        throw ExcepcionJSON("El campo 'numero_mesa' es inválido (<= 0)");
    }

    if (pedido.id_recepcionista <= 0) {
        throw ExcepcionJSON("El campo 'id_recepcionista' es inválido (<= 0)");
    }

    if (estadoPedidoToString(pedido.estado_general).isEmpty()) {
        throw ExcepcionJSON("El estado_general no es válido");
    }

    QJsonObject json;
    json["id_pedido"] = QJsonValue::fromVariant(QVariant::fromValue(pedido.id_pedido));
    json["numero_mesa"] = pedido.numero_mesa;
    json["id_recepcionista"] = pedido.id_recepcionista;
    json["estado_general"] = estadoPedidoToString(pedido.estado_general);

    QJsonArray platosArray;
    for (const auto& plato : pedido.platos) {
        platosArray.append(platoInstanciaToJson(plato));
    }

    json["platos"] = platosArray;

    return json;
}

PedidoMesa SerializadorJSON::jsonToPedidoMesa(const QJsonObject& json) {

    // Validación de campos obligatorios
    if (!json.contains("id_pedido")) throw ExcepcionCampoFaltante("id_pedido");
    if (!json.contains("numero_mesa")) throw ExcepcionCampoFaltante("numero_mesa");
    if (!json.contains("id_recepcionista")) throw ExcepcionCampoFaltante("id_recepcionista");
    if (!json.contains("estado_general")) throw ExcepcionCampoFaltante("estado_general");
    if (!json.contains("platos")) throw ExcepcionCampoFaltante("platos");

    // Validación de tipos
    if (!json["id_pedido"].isDouble())
        throw ExcepcionTipoIncorrecto("id_pedido", "long long", "otro tipo");

    if (!json["numero_mesa"].isDouble())
        throw ExcepcionTipoIncorrecto("numero_mesa", "int", "otro tipo");

    if (!json["id_recepcionista"].isDouble())
        throw ExcepcionTipoIncorrecto("id_recepcionista", "int", "otro tipo");

    if (!json["estado_general"].isString())
        throw ExcepcionTipoIncorrecto("estado_general", "string", "otro tipo");

    if (!json["platos"].isArray())
        throw ExcepcionTipoIncorrecto("platos", "array", "otro tipo");


    // Construcción segura del objeto
    PedidoMesa pedido;
    pedido.id_pedido = json["id_pedido"].toVariant().toLongLong();
    pedido.numero_mesa = json["numero_mesa"].toInt();
    pedido.id_recepcionista = json["id_recepcionista"].toInt();
    pedido.estado_general = stringToEstadoPedido(json["estado_general"].toString());

    QJsonArray platosArray = json["platos"].toArray();
    for (const QJsonValue& val : platosArray) {
        if (!val.isObject())
            throw ExcepcionTipoIncorrecto("plato", "object", "otro tipo");

        pedido.platos.push_back(jsonToPlatoInstancia(val.toObject()));
    }

    return pedido;
}

// Implementación de helpers para Enums
QString SerializadorJSON::estadoPlatoToString(EstadoPlato estado) {
    switch(estado) {
        case EstadoPlato::EN_ESPERA: return "EN_ESPERA";
        case EstadoPlato::EN_PROGRESO: return "EN_PROGRESO";
        case EstadoPlato::FINALIZADO: return "FINALIZADO";
        case EstadoPlato::CANCELADO: return "CANCELADO";
        case EstadoPlato::ENTREGADO: return "ENTREGADO";
        case EstadoPlato::DEVUELTO: return "DEVUELTO";
        default: throw ExcepcionValorEnumInvalido("EstadoPlato");
    }
}

EstadoPlato SerializadorJSON::stringToEstadoPlato(const QString& str) {
    if (str == "EN_ESPERA") return EstadoPlato::EN_ESPERA;
    if (str == "EN_PROGRESO") return EstadoPlato::EN_PROGRESO;
    if (str == "FINALIZADO") return EstadoPlato::FINALIZADO;
    if (str == "CANCELADO") return EstadoPlato::CANCELADO;
    if (str == "ENTREGADO") return EstadoPlato::ENTREGADO;

    throw ExcepcionValorEnumInvalido("EstadoPlato");
}

QString SerializadorJSON::estadoPedidoToString(EstadoPedido estado) {
    switch(estado) {
        case EstadoPedido::PENDIENTE: return "PENDIENTE";
        case EstadoPedido::EN_PROGRESO: return "EN_PROGRESO";
        case EstadoPedido::LISTO: return "LISTO";
        case EstadoPedido::ENTREGADO: return "ENTREGADO";
        case EstadoPedido::CANCELADO: return "CANCELADO";
        default: throw ExcepcionValorEnumInvalido("EstadoPedido");
    }
}

EstadoPedido SerializadorJSON::stringToEstadoPedido(const QString& str) {
    if (str == "PENDIENTE") return EstadoPedido::PENDIENTE;
    if (str == "EN_PROGRESO") return EstadoPedido::EN_PROGRESO;
    if (str == "LISTO") return EstadoPedido::LISTO;
    if (str == "ENTREGADO" || str == "COMPLETADO") return EstadoPedido::ENTREGADO;
    if (str == "CANCELADO") return EstadoPedido::CANCELADO;

    throw ExcepcionValorEnumInvalido("EstadoPedido");
}

QJsonObject SerializadorJSON::infoPlatoPrioridadToJson(const InfoPlatoPrioridad& info) {
    QJsonObject json;
    json["id_pedido"] = QJsonValue::fromVariant(info.id_pedido);
    json["id_instancia"] = QJsonValue::fromVariant(info.id_instancia_plato);
    json["score"] = info.score_prioridad;
    return json;
}

InfoPlatoPrioridad SerializadorJSON::jsonToInfoPlatoPrioridad(const QJsonObject& json) {

    // Validación de campos obligatorios
    if (!json.contains("id_pedido")) throw ExcepcionCampoFaltante("id_pedido");
    if (!json.contains("id_instancia")) throw ExcepcionCampoFaltante("id_instancia");
    if (!json.contains("score")) throw ExcepcionCampoFaltante("score");

    // Validación de tipos
    if (!json["id_pedido"].isDouble())
        throw ExcepcionTipoIncorrecto("id_pedido", "long long", "otro tipo");

    if (!json["id_instancia"].isDouble())
        throw ExcepcionTipoIncorrecto("id_instancia", "long long", "otro tipo");

    if (!json["score"].isDouble())
        throw ExcepcionTipoIncorrecto("score", "double", "otro tipo");

    // Construcción segura del objeto
    InfoPlatoPrioridad info;
    info.id_pedido = json["id_pedido"].toVariant().toLongLong();
    info.id_instancia_plato = json["id_instancia"].toVariant().toLongLong();
    info.score_prioridad = json["score"].toDouble();

    // Validación de valores
    if (info.id_pedido <= 0)
        throw ExcepcionModeloInvalido("InfoPlatoPrioridad", "El ID de pedido debe ser mayor que 0");
    if (info.id_instancia_plato <= 0)
        throw ExcepcionModeloInvalido("InfoPlatoPrioridad", "El ID de instancia de plato debe ser mayor que 0");
    if (info.score_prioridad < 0)
        throw ExcepcionModeloInvalido("InfoPlatoPrioridad", "El score de prioridad no puede ser negativo");

    return info;
}
