#include "JsonMenuRepository.h"
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

JsonMenuRepository::JsonMenuRepository(const QString& rutaArchivo)
    : m_rutaArchivo(rutaArchivo) {}

QJsonArray JsonMenuRepository::obtenerMenu() {
    QFile file(m_rutaArchivo);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "No se pudo abrir el archivo del menú:" << m_rutaArchivo;
        return QJsonArray();
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (!doc.isArray()) {
        qWarning() << "El archivo no contiene un array JSON válido.";
        return QJsonArray();
    }

    return doc.array();
}
