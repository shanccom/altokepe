#ifndef ESTRATEGIA_PLATO_TERMINADO_H
#define ESTRATEGIA_PLATO_TERMINADO_H

#include "IEstrategia.h"

class EstrategiaMarcarPlatoTerminado : public IEstrategia {
public:
    void ejecutar(LogicaNegocio* logica,
                  const QJsonObject& mensaje,
                  ManejadorCliente* remitente) override;
};

#endif
