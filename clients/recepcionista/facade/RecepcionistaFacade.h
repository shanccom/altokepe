/**
 * PATRÓN FACADE - RecepcionistaFacade
 * 
 * PROPÓSITO:
 * Proporcionar una interfaz simplificada y unificada para el subsistema de red
 * del cliente recepcionista, ocultando la complejidad de ClienteRecepcionista
 * y la gestión de conexiones TCP.
 * 
 * PROBLEMA QUE RESUELVE:
 * - PanelPedido creaba su propia instancia de ClienteRecepcionista
 * - Múltiples conexiones TCP innecesarias al servidor
 * - Alto acoplamiento entre UI y capa de red
 * - Difícil de testear y mantener
 * 
 * SOLUCIÓN:
 * - Centraliza la gestión de red en un único punto (Facade)
 * - Una sola conexión TCP compartida
 * - Interfaz simple: conectar(), enviarPedido(), obtenerMenu()
 * - Bajo acoplamiento mediante inyección de dependencias
 */

#pragma once

#include <QObject>
#include <QJsonArray>
#include <QString>

// Forward declaration - evita dependencia circular y reduce acoplamiento
class ClienteRecepcionista;

class RecepcionistaFacade : public QObject {
    Q_OBJECT

public:
    explicit RecepcionistaFacade(QObject *parent = nullptr);
    
    // Interfaz simplificada para clientes UI
    void conectarAlServidor(const QString &host, quint16 puerto);
    void enviarNuevoPedido(int mesa, int idRecepcionista, const QJsonArray &platos);
    QJsonArray obtenerMenu() const;

signals:
    // Señales que el Facade reenvía desde el subsistema interno
    void menuActualizado(const QJsonArray &menu);
    void conexionEstablecida();
    void errorConexion(const QString &mensaje);

private:
    // Subsistema encapsulado - oculto de los clientes
    ClienteRecepcionista *clienteInterno;
    
    // Cache local del menú
    QJsonArray menuActual;

private slots:
    // Procesa y reenvía señales del subsistema
    void procesarActualizacionMenu(const QJsonArray &menu);
};
