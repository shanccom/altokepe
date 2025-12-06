#pragma once
#include "MenuRepository.h"
#include <QString>

class JsonMenuRepository : public MenuRepository {
public:
    explicit JsonMenuRepository(const QString& rutaArchivo);
    QJsonArray obtenerMenu() override;

private:
    QString m_rutaArchivo;
};
