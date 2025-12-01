#ifndef ISERIALIZADOR_H
#define ISERIALIZADOR_H

#include <QJsonObject>
#include "../models/PedidoMesa.h"
#include "../models/PlatoDefinicion.h"
#include "../models/InfoPlatoPrioridad.h"

/**
 * @brief Interfaz genérica para serialización de modelos (Target del patrón Adapter)
 * 
 * Esta interfaz define el contrato para serializar y deserializar los modelos
 * del sistema. Permite desacoplar el código cliente del formato específico de
 * serialización (JSON, XML, Protobuf, etc.).
 */
class ISerializador {
public:
  virtual ~ISerializador() = default;

  // PlatoDefinicion
  virtual QJsonObject platoDefinicionToJson(const PlatoDefinicion& plato) = 0;
  virtual PlatoDefinicion jsonToPlatoDefinicion(const QJsonObject& json) = 0;
  
  // PlatoInstancia
  virtual QJsonObject platoInstanciaToJson(const PlatoInstancia& plato) = 0;
  virtual PlatoInstancia jsonToPlatoInstancia(const QJsonObject& json) = 0;

  // PedidoMesa
  virtual QJsonObject pedidoMesaToJson(const PedidoMesa& pedido) = 0;
  virtual PedidoMesa jsonToPedidoMesa(const QJsonObject& json) = 0;

  // InfoPlatoPrioridad
  virtual QJsonObject infoPlatoPrioridadToJson(const InfoPlatoPrioridad& info) = 0;
  virtual InfoPlatoPrioridad jsonToInfoPlatoPrioridad(const QJsonObject& json) = 0;

  // Helpers para Enums
  virtual QString estadoPlatoToString(EstadoPlato estado) = 0;
  virtual EstadoPlato stringToEstadoPlato(const QString& str) = 0;

  virtual QString estadoPedidoToString(EstadoPedido estado) = 0;
  virtual EstadoPedido stringToEstadoPedido(const QString& str) = 0;
};

#endif
