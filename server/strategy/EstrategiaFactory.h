#ifndef ESTRATEGIA_FACTORY_H
#define ESTRATEGIA_FACTORY_H

#include <QString>
#include "IEstrategia.h"

class EstrategiaFactory {
public:
    static IEstrategia* obtener(const QString& evento);
};

#endif
