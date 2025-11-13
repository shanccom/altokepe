#ifndef CLIENTEMANAGERAPP_H
#define CLIENTEMANAGERAPP_H

#include <QObject>

class ClienteTCP;

class ClienteManagerApp : public QObject {
  Q_OBJECT
public:
  explicit ClienteManagerApp(QObject* parent = nullptr);
  void iniciar();

private slots:
  void onConectado();
  void onMensajeRecibido(const QJsonObject& mensaje);

private:
  ClienteTCP* m_clienteTCP;
};

#endif
