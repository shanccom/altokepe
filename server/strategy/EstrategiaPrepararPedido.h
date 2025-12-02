#ifndef ESTRATEGIA_PREPARAR_PEDIDO_H
#define ESTRATEGIA_PREPARAR_PEDIDO_H

#include "IEstrategia.h"

class EstrategiaPrepararPedido : public IEstrategia {
public:
    void ejecutar(LogicaNegocio* logica,
                  const QJsonObject& mensaje,
                  ManejadorCliente* remitente) override;
};

#endif
