#ifndef CLIENTETCP_ESTACION_H
#define CLIENTETCP_ESTACION_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class ClienteTCP : public QObject {
    Q_OBJECT
public:
    explicit ClienteTCP(QObject* parent = nullptr);
    void conectar(const QString& host, quint16 puerto);
    void enviarMensaje(const QJsonObject& mensaje);

signals:
    void conectado();
    void desconectado();
    void nuevoMensajeRecibido(const QJsonObject& mensaje);

private slots:
    void onConectado();
    void onDesconectado();
    void leerDatos();

private:
    QTcpSocket* socket;
    QByteArray buffer;
};

#endif // CLIENTETCP_ESTACION_H