#ifndef ESTRATEGIA_NUEVO_PEDIDO_H
#define ESTRATEGIA_NUEVO_PEDIDO_H

#include "IEstrategia.h"

class EstrategiaNuevoPedido : public IEstrategia {
public:
    void ejecutar(LogicaNegocio* logica,
                  const QJsonObject& mensaje,
                  ManejadorCliente* remitente) override;
};

#endif
