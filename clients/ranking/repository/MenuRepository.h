#pragma once
#include <QJsonArray>

class MenuRepository {
public:
    virtual ~MenuRepository() = default;
    virtual QJsonArray obtenerMenu() = 0;
};
