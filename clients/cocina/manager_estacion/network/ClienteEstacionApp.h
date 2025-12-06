#ifndef CLIENTEESTACIONAPP_H
#define CLIENTEESTACIONAPP_H

#include <QObject>
#include <unordered_map>
#include "common/models/PlatoDefinicion.h"
#include "common/models/InfoPlatoPrioridad.h"
#include "../ui/VentanaEstacion.h"

class ClienteTCP;

class ClienteEstacionApp : public QObject {
    Q_OBJECT
public:
    explicit ClienteEstacionApp(const QString& nombreEstacion, QObject* parent = nullptr);
    void iniciar();
    QWidget* getVentana() const { return m_ventana; }
private slots:
    void onConectado();
    void onMensajeRecibido(const QJsonObject& mensaje);
    void onMarcarListo(long long idPedido, long long idInstancia);

private:
    QString m_nombreEstacion;
    VentanaEstacion* m_ventana;
    ClienteTCP* m_clienteTCP;
    std::unordered_map<int, PlatoDefinicion> m_menu;
};

#endif