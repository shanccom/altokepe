#pragma once

#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonArray>

class ClienteRecepcionista : public QObject {
    Q_OBJECT
public:
    ClienteRecepcionista(QObject *parent = nullptr);
    void conectarAlServidor(const QString &host, quint16 puerto);
    void enviarNuevoPedido(int mesa, int idRecepcionista, const QJsonArray &platos);

private:
    QTcpSocket *socket;
    QJsonArray menu;

private slots:
    void leerMensaje();

signals:
    void menuActualizado(const QJsonArray &menu);
};
