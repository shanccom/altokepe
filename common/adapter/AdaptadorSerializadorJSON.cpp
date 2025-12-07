#include "AdaptadorSerializadorJSON.h"
#include "../ExcepcionesCommon.h"
#include <QDebug>

// PlatoDefinicion
QJsonObject AdaptadorSerializadorJSON::platoDefinicionToJson(const PlatoDefinicion& plato) {
    try {
        return SerializadorJSON::platoDefinicionToJson(plato);
    }
    catch (const ExcepcionCommon& e) {
        qWarning() << "Adaptador: Error al serializar PlatoDefinicion:" << e.what();
        throw ExcepcionAdaptador("platoDefinicionToJson", e.obtenerMensaje());
    }
    catch (const std::exception& e) {
        qWarning() << "Adaptador: Error inesperado al serializar PlatoDefinicion:" << e.what();
        throw ExcepcionAdaptador("platoDefinicionToJson", e.what());
    }
}

PlatoDefinicion AdaptadorSerializadorJSON::jsonToPlatoDefinicion(const QJsonObject& json) {
    return SerializadorJSON::jsonToPlatoDefinicion(json);
}

// PlatoInstancia
QJsonObject AdaptadorSerializadorJSON::platoInstanciaToJson(const PlatoInstancia& plato) {
    return SerializadorJSON::platoInstanciaToJson(plato);
}

PlatoInstancia AdaptadorSerializadorJSON::jsonToPlatoInstancia(const QJsonObject& json) {
    return SerializadorJSON::jsonToPlatoInstancia(json);
}

// PedidoMesa
QJsonObject AdaptadorSerializadorJSON::pedidoMesaToJson(const PedidoMesa& pedido) {
    return SerializadorJSON::pedidoMesaToJson(pedido);
}

PedidoMesa AdaptadorSerializadorJSON::jsonToPedidoMesa(const QJsonObject& json) {
    return SerializadorJSON::jsonToPedidoMesa(json);
}

// InfoPlatoPrioridad
QJsonObject AdaptadorSerializadorJSON::infoPlatoPrioridadToJson(const InfoPlatoPrioridad& info) {
    return SerializadorJSON::infoPlatoPrioridadToJson(info);
}

InfoPlatoPrioridad AdaptadorSerializadorJSON::jsonToInfoPlatoPrioridad(const QJsonObject& json) {
    return SerializadorJSON::jsonToInfoPlatoPrioridad(json);
}

// Helpers para Enums
QString AdaptadorSerializadorJSON::estadoPlatoToString(EstadoPlato estado) {
    return SerializadorJSON::estadoPlatoToString(estado);
}

EstadoPlato AdaptadorSerializadorJSON::stringToEstadoPlato(const QString& str) {
    return SerializadorJSON::stringToEstadoPlato(str);
}

QString AdaptadorSerializadorJSON::estadoPedidoToString(EstadoPedido estado) {
    return SerializadorJSON::estadoPedidoToString(estado);
}

EstadoPedido AdaptadorSerializadorJSON::stringToEstadoPedido(const QString& str) {
    return SerializadorJSON::stringToEstadoPedido(str);
}
