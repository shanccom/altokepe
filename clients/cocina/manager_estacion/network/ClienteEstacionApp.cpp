#include "ClienteEstacionApp.h"
#include "ClienteTCP.h"
#include "../ui/VentanaEstacion.h" 
#include "common/network/Protocolo.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

ClienteEstacionApp::ClienteEstacionApp(const QString& nombreEstacion, QObject* parent)
    : QObject(parent), m_nombreEstacion(nombreEstacion) {
    
    m_ventana = new VentanaEstacion(nombreEstacion, qobject_cast<QWidget*>(parent));
    m_clienteTCP = new ClienteTCP(this);

    // Red
    connect(m_clienteTCP, &ClienteTCP::conectado, this, &ClienteEstacionApp::onConectado);
    connect(m_clienteTCP, &ClienteTCP::nuevoMensajeRecibido, this, &ClienteEstacionApp::onMensajeRecibido);

    // UI -> Lógica
    connect(m_ventana, &VentanaEstacion::marcarListoSolicitado, this, &ClienteEstacionApp::onMarcarListo);
}

void ClienteEstacionApp::iniciar() {
    m_clienteTCP->conectar("127.0.0.1", 5555);
    m_ventana->show();
}

void ClienteEstacionApp::onConectado() {
    qDebug() << "[Estacion] Conectado como:" << m_nombreEstacion;

    QJsonObject identificacion;
    identificacion[Protocolo::COMANDO] = Protocolo::IDENTIFICARSE;
    identificacion["rol"] = "EstacionCocina";
    identificacion["nombre_estacion"] = m_nombreEstacion;
    m_clienteTCP->enviarMensaje(identificacion);
}

void ClienteEstacionApp::onMensajeRecibido(const QJsonObject& mensaje) {
    const QString evento = mensaje[Protocolo::EVENTO].toString();
    const QJsonObject data = mensaje[Protocolo::DATA].toObject();

    if (evento == Protocolo::ACTUALIZACION_ESTADO_ESTACION) {
        QJsonArray platosJson = data["platos_pendientes"].toArray();
        std::vector<VentanaEstacion::InfoPlatoVisual> platos;

        for (const QJsonValue& val : platosJson) {
            QJsonObject obj = val.toObject();
            VentanaEstacion::InfoPlatoVisual visual;
            visual.nombrePlato = obj["nombre"].toString();
            visual.estado = "EN_ESPERA";
            visual.prioridad = obj["score"].toDouble();
            visual.id_pedido = obj["id_pedido"].toVariant().toLongLong();
            visual.id_instancia = obj["id_instancia"].toVariant().toLongLong();
            platos.push_back(visual);
        }

        m_ventana->cargarPlatosIniciales(platos);

    } else if (evento == Protocolo::NUEVO_PLATO_EN_COLA) {
        QString estacionDestino = data["estacion"].toString();
        if (!estacionDestino.isEmpty() && estacionDestino != m_nombreEstacion) return;

        VentanaEstacion::InfoPlatoVisual visual;
        visual.nombrePlato = data["nombre"].toString();
        visual.estado = data["nuevo_estado"].toString();
        visual.prioridad = data["score"].toDouble();
        visual.id_pedido = data["id_pedido"].toVariant().toLongLong();
        visual.id_instancia = data["id_instancia"].toVariant().toLongLong();

        m_ventana->agregarNuevoPlato(visual);

    } else if (evento == Protocolo::PLATO_ESTADO_CAMBIADO) {
        long long idInstancia = data["id_instancia"].toVariant().toLongLong();
        QString estado = data["nuevo_estado"].toString();

        qDebug() << "[DEBUG] Estado recibido para cambio:" << estado
                 << "Instancia:" << idInstancia;

        m_ventana->actualizarEstadoPlato(idInstancia, estado);
    }

}

void ClienteEstacionApp::onMarcarListo(long long idPedido, long long idInstancia) {
    QJsonObject comando;
    comando[Protocolo::COMANDO] = Protocolo::MARCAR_PLATO_TERMINADO;

    QJsonObject data;
    data["id_pedido"]   = QJsonValue::fromVariant(idPedido);
    data["id_instancia"] = QJsonValue::fromVariant(idInstancia);
    comando[Protocolo::DATA] = data;

    m_clienteTCP->enviarMensaje(comando);
}
