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
    try {
        return SerializadorJSON::jsonToPlatoDefinicion(json);
    }
    catch (const ExcepcionCommon& e) {
        qWarning() << "Adaptador: Error al deserializar JSON a PlatoDefinicion:" << e.what();
        throw ExcepcionAdaptador("jsonToPlatoDefinicion", e.obtenerMensaje());
    }
    catch (const std::exception& e) {
        qWarning() << "Adaptador: Error inesperado al deserializar JSON a PlatoDefinicion:" << e.what();
        throw ExcepcionAdaptador("jsonToPlatoDefinicion", e.what());
    }
}

// PlatoInstancia
QJsonObject AdaptadorSerializadorJSON::platoInstanciaToJson(const PlatoInstancia& plato) {
    try {
        return SerializadorJSON::platoInstanciaToJson(plato);
    }
    catch (const ExcepcionCommon& e) {
        qWarning() << "Adaptador: Error al serializar PlatoInstancia:" << e.what();
        throw ExcepcionAdaptador("platoInstanciaToJson", e.obtenerMensaje());
    }
    catch (const std::exception& e) {
        qWarning() << "Adaptador: Error inesperado al serializar PlatoInstancia:" << e.what();
        throw ExcepcionAdaptador("platoInstanciaToJson", e.what());
    }
}

PlatoInstancia AdaptadorSerializadorJSON::jsonToPlatoInstancia(const QJsonObject& json) {
    try {
        return SerializadorJSON::jsonToPlatoInstancia(json);
    }
    catch (const ExcepcionCommon& e) {
        qWarning() << "Adaptador: Error al deserializar JSON a PlatoInstancia:" << e.what();
        throw ExcepcionAdaptador("jsonToPlatoInstancia", e.obtenerMensaje());
    }
    catch (const std::exception& e) {
        qWarning() << "Adaptador: Error inesperado al deserializar JSON a PlatoInstancia:" << e.what();
        throw ExcepcionAdaptador("jsonToPlatoInstancia", e.what());
    }
}
// PedidoMesa
QJsonObject AdaptadorSerializadorJSON::pedidoMesaToJson(const PedidoMesa& pedido) {
    try {
        return SerializadorJSON::pedidoMesaToJson(pedido);
    }
    catch (const ExcepcionCommon& e) {
        qWarning() << "Adaptador: Error al serializar PedidoMesa:" << e.what();
        throw ExcepcionAdaptador("pedidoMesaToJson", e.obtenerMensaje());
    }
    catch (const std::exception& e) {
        qWarning() << "Adaptador: Error inesperado al serializar PedidoMesa:" << e.what();
        throw ExcepcionAdaptador("pedidoMesaToJson", e.what());
    }
}

PedidoMesa AdaptadorSerializadorJSON::jsonToPedidoMesa(const QJsonObject& json) {
    try {
        return SerializadorJSON::jsonToPedidoMesa(json);
    }
    catch (const ExcepcionCommon& e) {
        qWarning() << "Adaptador: Error al deserializar JSON a PedidoMesa:" << e.what();
        throw ExcepcionAdaptador("jsonToPedidoMesa", e.obtenerMensaje());
    }
    catch (const std::exception& e) {
        qWarning() << "Adaptador: Error inesperado al deserializar JSON a PedidoMesa:" << e.what();
        throw ExcepcionAdaptador("jsonToPedidoMesa", e.what());
    }
}


// InfoPlatoPrioridad
QJsonObject AdaptadorSerializadorJSON::infoPlatoPrioridadToJson(const InfoPlatoPrioridad& info) {
    try {
        return SerializadorJSON::infoPlatoPrioridadToJson(info);
    }
    catch (const ExcepcionCommon& e) {
        qWarning() << "Adaptador: Error al serializar InfoPlatoPrioridad:" << e.what();
        throw ExcepcionAdaptador("infoPlatoPrioridadToJson", e.obtenerMensaje());
    }
    catch (const std::exception& e) {
        qWarning() << "Adaptador: Error inesperado al serializar InfoPlatoPrioridad:" << e.what();
        throw ExcepcionAdaptador("infoPlatoPrioridadToJson", e.what());
    }
}

InfoPlatoPrioridad AdaptadorSerializadorJSON::jsonToInfoPlatoPrioridad(const QJsonObject& json) {
    try {
        return SerializadorJSON::jsonToInfoPlatoPrioridad(json);
    }
    catch (const ExcepcionCommon& e) {
        qWarning() << "Adaptador: Error al deserializar JSON a InfoPlatoPrioridad:" << e.what();
        throw ExcepcionAdaptador("jsonToInfoPlatoPrioridad", e.obtenerMensaje());
    }
    catch (const std::exception& e) {
        qWarning() << "Adaptador: Error inesperado al deserializar JSON a InfoPlatoPrioridad:" << e.what();
        throw ExcepcionAdaptador("jsonToInfoPlatoPrioridad", e.what());
    }
}


// Helpers para Enums
QString AdaptadorSerializadorJSON::estadoPlatoToString(EstadoPlato estado) {
    try {
        return SerializadorJSON::estadoPlatoToString(estado);
    }
    catch (const ExcepcionCommon& e) {
        qWarning() << "Adaptador: Error al convertir EstadoPlato a string:" << e.what();
        throw ExcepcionAdaptador("estadoPlatoToString", e.obtenerMensaje());
    }
    catch (const std::exception& e) {
        qWarning() << "Adaptador: Error inesperado al convertir EstadoPlato a string:" << e.what();
        throw ExcepcionAdaptador("estadoPlatoToString", e.what());
    }
}

EstadoPlato AdaptadorSerializadorJSON::stringToEstadoPlato(const QString& str) {
    try {
        return SerializadorJSON::stringToEstadoPlato(str);
    }
    catch (const ExcepcionCommon& e) {
        qWarning() << "Adaptador: Error al convertir string a EstadoPlato:" << e.what();
        throw ExcepcionAdaptador("stringToEstadoPlato", e.obtenerMensaje());
    }
    catch (const std::exception& e) {
        qWarning() << "Adaptador: Error inesperado al convertir string a EstadoPlato:" << e.what();
        throw ExcepcionAdaptador("stringToEstadoPlato", e.what());
    }
}

QString AdaptadorSerializadorJSON::estadoPedidoToString(EstadoPedido estado) {
    try {
        return SerializadorJSON::estadoPedidoToString(estado);
    }
    catch (const ExcepcionCommon& e) {
        qWarning() << "Adaptador: Error al convertir EstadoPedido a string:" << e.what();
        throw ExcepcionAdaptador("estadoPedidoToString", e.obtenerMensaje());
    }
    catch (const std::exception& e) {
        qWarning() << "Adaptador: Error inesperado al convertir EstadoPedido a string:" << e.what();
        throw ExcepcionAdaptador("estadoPedidoToString", e.what());
    }
}

EstadoPedido AdaptadorSerializadorJSON::stringToEstadoPedido(const QString& str) {
    try {
        return SerializadorJSON::stringToEstadoPedido(str);
    }
    catch (const ExcepcionCommon& e) {
        qWarning() << "Adaptador: Error al convertir string a EstadoPedido:" << e.what();
        throw ExcepcionAdaptador("stringToEstadoPedido", e.obtenerMensaje());
    }
    catch (const std::exception& e) {
        qWarning() << "Adaptador: Error inesperado al convertir string a EstadoPedido:" << e.what();
        throw ExcepcionAdaptador("stringToEstadoPedido", e.what());
    }
}
