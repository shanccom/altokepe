#include "MenuRepository.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

MenuRepository::MenuRepository(AdaptadorSerializadorJSON& serializador)
    : m_serializador(serializador) {}

bool MenuRepository::cargarDesdeArchivo(const QString& rutaArchivo) {
    QFile archivo(rutaArchivo);
    if (!archivo.open(QIODevice::ReadOnly)) {
        qCritical() << "No se pudo abrir el archivo de menú:" << rutaArchivo;
        return false;
    }

    QByteArray data = archivo.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        qCritical() << "El archivo de menú no es un array JSON válido.";
        return false;
    }

    m_menu.clear();
    QJsonArray menuArray = doc.array();
    for (const QJsonValue& val : menuArray) {
        PlatoDefinicion plato = m_serializador.jsonToPlatoDefinicion(val.toObject());
        m_menu[plato.id] = plato;
    }

    qInfo() << "Menú cargado desde" << rutaArchivo << "con" << m_menu.size() << "platos.";
    return true;
}

const PlatoDefinicion* MenuRepository::obtenerPlato(int id) const {
    auto it = m_menu.find(id);
    if (it == m_menu.end()) return nullptr;
    return &it->second;
}

const std::unordered_map<int, PlatoDefinicion>& MenuRepository::menu() const {
    return m_menu;
}

QJsonArray MenuRepository::menuComoJson() const {
    QJsonArray arr;
    for (const auto& [id, plato] : m_menu) {
        arr.append(SerializadorJSON::platoDefinicionToJson(plato));
    }
    return arr;
}
