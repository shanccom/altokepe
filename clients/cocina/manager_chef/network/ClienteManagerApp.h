#ifndef CLIENTEMANAGERAPP_H
#define CLIENTEMANAGERAPP_H

#include <QObject>
#include <unordered_map>
#include "common/models/PlatoDefinicion.h"

class VentanaManager;
class ClienteTCP;

class ClienteManagerApp : public QObject {
  Q_OBJECT
public:
  explicit ClienteManagerApp(QObject* parent = nullptr);
  void iniciar();

private slots:
  void onConectado();
  void onMensajeRecibido(const QJsonObject& mensaje);
  void onPrepararPedido(long long idPedido);
  void onCancelarPedido(long long idPedido);
  void onEnviarPedido(long long idPedido);
  void onRechazarPedido(long long idPedido);

private:
  VentanaManager* m_ventana;
  ClienteTCP* m_clienteTCP;
  std::unordered_map<int, PlatoDefinicion> m_menu;
};

#endif
