/**
 * PATRÓN FACADE - Implementación
 * 
 * Este archivo implementa el Facade que simplifica el acceso al subsistema
 * de red, actuando como intermediario entre la UI y ClienteRecepcionista.
 */

#include "RecepcionistaFacade.h"
#include "../network/ClienteRecepcionista.h"
#include <QDebug>

/**
 * Constructor del Facade
 * 
 * RESPONSABILIDAD:
 * - Inicializar el subsistema interno (ClienteRecepcionista)
 * - Configurar conexiones de señales
 * 
 * BENEFICIO:
 * Los clientes (UI) no necesitan conocer estos detalles de inicialización
 */
RecepcionistaFacade::RecepcionistaFacade(QObject *parent)
    : QObject(parent),
      clienteInterno(new ClienteRecepcionista(this))
{
    // Conectar señales del subsistema con slots del Facade
    // Esto permite al Facade procesar y reenviar información
    connect(clienteInterno, &ClienteRecepcionista::menuActualizado,
            this, &RecepcionistaFacade::procesarActualizacionMenu);
    
    qDebug() << "[FACADE] RecepcionistaFacade inicializado";
}

/**
 * Conectar al servidor - Método simplificado
 * 
 * ENCAPSULA:
 * - Conexión TCP
 * - Identificación en el servidor
 * - Manejo de errores
 * 
 * POR QUÉ:
 * La UI no debe conocer detalles de IP, puertos o protocolos
 */
void RecepcionistaFacade::conectarAlServidor(const QString &host, quint16 puerto)
{
    qDebug() << "[FACADE] Conectando al servidor:" << host << ":" << puerto;
    
    // Delegar al subsistema interno
    clienteInterno->conectarAlServidor(host, puerto);
    
    // Aquí se podría agregar lógica adicional:
    // - Reconexión automática
    // - Verificación de estado
    // - Notificaciones de conexión
}

/**
 * Enviar pedido - Método simplificado
 * 
 * ENCAPSULA:
 * - Creación de mensaje JSON
 * - Envío por socket TCP
 * - Protocolo de comunicación
 * 
 * POR QUÉ:
 * PanelPedido solo debe proporcionar datos del pedido,
 * no conocer cómo se serializan o envían
 */
void RecepcionistaFacade::enviarNuevoPedido(int mesa, int idRecepcionista, const QJsonArray &platos)
{
    qDebug() << "[FACADE] Enviando pedido - Mesa:" << mesa 
             << "Recepcionista:" << idRecepcionista 
             << "Platos:" << platos.size();
    
    // Delegar al subsistema
    clienteInterno->enviarNuevoPedido(mesa, idRecepcionista, platos);
    
    // Aquí se podría agregar:
    // - Validación de datos
    // - Cache de pedidos
    // - Logging/auditoría
}

/**
 * Obtener menú - Método de consulta
 * 
 * BENEFICIO:
 * Acceso rápido al menú sin necesidad de conocer
 * la fuente de datos (red, cache, archivo, etc.)
 */
QJsonArray RecepcionistaFacade::obtenerMenu() const
{
    return menuActual;
}

/**
 * Procesar actualización del menú - Slot privado
 * 
 * RESPONSABILIDAD:
 * - Actualizar cache local
 * - Reenviar señal a clientes UI
 * 
 * POR QUÉ ES UN INTERMEDIARIO:
 * Permite agregar lógica de procesamiento sin que
 * la UI o el subsistema lo sepan (filtrado, ordenamiento, validación)
 */
void RecepcionistaFacade::procesarActualizacionMenu(const QJsonArray &menu)
{
    qDebug() << "[FACADE] Procesando actualización de menú - Items:" << menu.size();
    
    // Actualizar cache local
    menuActual = menu;
    
    // Posibles procesamientos aquí:
    // - Filtrar platos no disponibles
    // - Ordenar por categoría
    // - Validar estructura JSON
    
    // Reenviar señal a clientes
    emit menuActualizado(menu);
}
