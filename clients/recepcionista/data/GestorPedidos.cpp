#include "GestorPedidos.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

GestorPedidos gestorPedidos;

static const char* HISTORIAL_FILENAME = "historial_pedidos.json";

void GestorPedidos::cargarDesdeHistorial() {
    QFile f(HISTORIAL_FILENAME);
    if (!f.exists()) {
        return; // Silencioso para evitar spam
    }
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "[GestorPedidos] No se pudo abrir" << HISTORIAL_FILENAME;
        return;
    }
    QByteArray data = f.readAll();
    f.close();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "[GestorPedidos] Formato inválido historial.";
    }
}

void GestorPedidos::agregarEntrada(const QJsonArray& platos) {
    QJsonObject root;
    root["ultimo_pedido"] = platos;
    QJsonDocument doc(root);
    QFile f(HISTORIAL_FILENAME);
    if (!f.open(QIODevice::WriteOnly)) {
        qWarning() << "[GestorPedidos] No se pudo escribir" << HISTORIAL_FILENAME;
        return;
    }
    f.write(doc.toJson(QJsonDocument::Compact));
    f.close();
}
