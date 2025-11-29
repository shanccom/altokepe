#ifndef ADAPTADORSERIALIZADORJSON_H
#define ADAPTADORSERIALIZADORJSON_H

#include "ISerializador.h"
#include "../network/SerializadorJSON.h"

/**
 * @brief Adaptador que implementa ISerializador usando SerializadorJSON (Adapter del patrón)
 * 
 * Esta clase actúa como Adapter entre la interfaz genérica ISerializador (Target)
 * y la implementación concreta SerializadorJSON (Adaptee). Permite usar SerializadorJSON
 * a través de la interfaz genérica, facilitando futuros cambios de formato.
 */
class AdaptadorSerializadorJSON : public ISerializador {
public:
  AdaptadorSerializadorJSON() = default;
  ~AdaptadorSerializadorJSON() override = default;

  // PlatoDefinicion
  QJsonObject platoDefinicionToJson(const PlatoDefinicion& plato) override;
  PlatoDefinicion jsonToPlatoDefinicion(const QJsonObject& json) override;
  
  // PlatoInstancia
  QJsonObject platoInstanciaToJson(const PlatoInstancia& plato) override;
  PlatoInstancia jsonToPlatoInstancia(const QJsonObject& json) override;

  // PedidoMesa
  QJsonObject pedidoMesaToJson(const PedidoMesa& pedido) override;
  PedidoMesa jsonToPedidoMesa(const QJsonObject& json) override;

  // InfoPlatoPrioridad
  QJsonObject infoPlatoPrioridadToJson(const InfoPlatoPrioridad& info) override;
  InfoPlatoPrioridad jsonToInfoPlatoPrioridad(const QJsonObject& json) override;

  // Helpers para Enums
  QString estadoPlatoToString(EstadoPlato estado) override;
  EstadoPlato stringToEstadoPlato(const QString& str) override;

  QString estadoPedidoToString(EstadoPedido estado) override;
  EstadoPedido stringToEstadoPedido(const QString& str) override;
};

#endif
