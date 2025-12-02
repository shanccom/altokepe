#ifndef ESTRATEGIA_CANCELAR_PEDIDO_H
#define ESTRATEGIA_CANCELAR_PEDIDO_H

#include "IEstrategia.h"

class EstrategiaCancelarPedido : public IEstrategia {
public:
    void ejecutar(LogicaNegocio* logica,
                  const QJsonObject& mensaje,
                  ManejadorCliente* remitente) override;
};

#endif
