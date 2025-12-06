#ifndef MENUREPOSITORY_H
#define MENUREPOSITORY_H

#include "common/models/PlatoDefinicion.h"
#include "common/network/SerializadorJSON.h"
#include <unordered_map>
#include <QJsonArray>


class MenuRepository {
public:
    MenuRepository(SerializadorJSON& serializador);

    bool cargarDesdeArchivo(const QString& rutaArchivo);

    const PlatoDefinicion* obtenerPlato(int id) const;
    const std::unordered_map<int, PlatoDefinicion>& menu() const;

    QJsonArray menuComoJson() const;

private:
    SerializadorJSON& m_serializador;
    std::unordered_map<int, PlatoDefinicion> m_menu;
};

#endif
