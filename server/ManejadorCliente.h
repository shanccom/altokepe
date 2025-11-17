#ifndef MANEJADORCLIENTE_H
#define MANEJADORCLIENTE_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class ManejadorCliente : public QObject {
    Q_OBJECT

public:
    explicit ManejadorCliente(qintptr socketDescriptor, QObject* parent = nullptr);
    virtual ~ManejadorCliente();

public slots:
    void procesar();
    void enviarMensaje(const QJsonObject& mensaje);

private slots:
    void listoParaLeer();
    void desconectado();

signals:
    void finished();

private:
    void procesarBuffer();

    qintptr m_socketDescriptor;
    QTcpSocket* m_socket = nullptr;
    QByteArray m_buffer;
};

#endif
