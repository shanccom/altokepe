#ifndef IESTRATEGIA_H
#define IESTRATEGIA_H

#include <QJsonObject>

class LogicaNegocio;
class ManejadorCliente;

class IEstrategia {
public:
    virtual ~IEstrategia() {}
    virtual void ejecutar(LogicaNegocio* logica,
                          const QJsonObject& mensaje,
                          ManejadorCliente* remitente) = 0;
};

#endif
