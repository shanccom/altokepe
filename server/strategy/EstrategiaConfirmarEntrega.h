#ifndef ESTRATEGIA_CONFIRMAR_ENTREGA_H
#define ESTRATEGIA_CONFIRMAR_ENTREGA_H

#include "IEstrategia.h"

class EstrategiaConfirmarEntrega : public IEstrategia {
public:
    void ejecutar(LogicaNegocio* logica,
                  const QJsonObject& mensaje,
                  ManejadorCliente* remitente) override;
};

#endif
