#ifndef ESTRATEGIA_DEVOLVER_PLATO_H
#define ESTRATEGIA_DEVOLVER_PLATO_H

#include "IEstrategia.h"

class EstrategiaDevolverPlato : public IEstrategia {
public:
    void ejecutar(LogicaNegocio* logica,
                  const QJsonObject& mensaje,
                  ManejadorCliente* remitente) override;
};

#endif
