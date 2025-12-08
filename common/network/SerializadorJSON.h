#ifndef SERIALIZADORJSON_H
#define SERIALIZADORJSON_H

#include <QJsonObject>
#include "../models/PedidoMesa.h"
#include "../models/PlatoDefinicion.h"
#include "../models/InfoPlatoPrioridad.h"
#include "../ExcepcionesCommon.h"

/**
 * @brief Serializador JSON para modelos del sistema
 * 
 * @throws ExcepcionCampoFaltante cuando falta un campo requerido en JSON
 * @throws ExcepcionTipoIncorrecto cuando un campo tiene tipo incorrecto
 * @throws ExcepcionSerializacion para otros errores de serialización
 */
class SerializadorJSON {
public:
  // PlatoDefinicion
  static QJsonObject platoDefinicionToJson(const PlatoDefinicion& plato);
  static PlatoDefinicion jsonToPlatoDefinicion(const QJsonObject& json);
  
  // PlatoInstancia
  static QJsonObject platoInstanciaToJson(const PlatoInstancia& plato);
  static PlatoInstancia jsonToPlatoInstancia(const QJsonObject& json);

  // PedidoMesa
  static QJsonObject pedidoMesaToJson(const PedidoMesa& pedido);
  static PedidoMesa jsonToPedidoMesa(const QJsonObject& json);

  // Helpers para Enums
  static QString estadoPlatoToString(EstadoPlato estado);
  static EstadoPlato stringToEstadoPlato(const QString& str);

  static QString estadoPedidoToString(EstadoPedido estado);
  static EstadoPedido stringToEstadoPedido(const QString& str);
  
  // InfoPlatoPrioridad
  static QJsonObject infoPlatoPrioridadToJson(const InfoPlatoPrioridad& info);
  static InfoPlatoPrioridad jsonToInfoPlatoPrioridad(const QJsonObject& json);

};

#endif