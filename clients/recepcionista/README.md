# Cliente Recepcionista - Sistema Altoke Pe

## 📋 Descripción

El módulo **Cliente Recepcionista** es la interfaz gráfica que permite a los recepcionistas del restaurante gestionar mesas y crear pedidos para los clientes.

##  Funcionalidad Básica

1. **Selección de Mesa**: El recepcionista puede ver un panel con 9 mesas disponibles y seleccionar una haciendo clic.
2. **Creación de Pedido**: Una vez seleccionada la mesa, puede:
   - Ingresar el nombre del cliente
   - Seleccionar platos del menú disponible
   - Especificar la cantidad de cada plato
   - Ver el total del pedido en tiempo real
3. **Envío del Pedido**: Al enviar el pedido:
   - Se transmite al servidor a través de la red (TCP/IP)
   - La mesa queda marcada como **no disponible** (color gris)
   - La mesa deshabilitada no puede ser seleccionada nuevamente

## 🏗️ Arquitectura

El proyecto está organizado en capas:

```
recepcionista/
├── ui/                        # Interfaz gráfica (Qt Widgets)
│   ├── VentanaRecepcionista   # Ventana principal
│   ├── PanelMesas             # Grid de botones de mesas
│   └── PanelPedido            # Formulario de pedido
├── facade/                    # Patrón Facade
│   └── RecepcionistaFacade    # Interfaz simplificada de red
├── network/                   # Capa de comunicación TCP
│   └── ClienteRecepcionista   # Cliente TCP
└── data/                      # Gestión de datos
    └── GestorPedidos          # Historial de pedidos
```

## 🎭 Patrón de Diseño: Facade

### Problema que Resuelve

**Antes de Facade:**
- Cada componente UI (`PanelPedido`) creaba su propia conexión TCP
- Múltiples sockets innecesarios al servidor
- Alto acoplamiento entre UI y capa de red
- Difícil de testear y mantener

**Después de Facade:**
- Una sola conexión TCP compartida
- Interfaz simple y uniforme
- Bajo acoplamiento mediante inyección de dependencias

### Implementación

#### 1. **RecepcionistaFacade** - La interfaz simplificada

```cpp
class RecepcionistaFacade : public QObject {
public:
    // Métodos simples que ocultan la complejidad interna
    void conectarAlServidor(const QString &host, quint16 puerto);
    void enviarNuevoPedido(int mesa, int idRecep, const QJsonArray &platos);
    QJsonArray obtenerMenu() const;

signals:
    void menuActualizado(const QJsonArray &menu);
    void conexionEstablecida();

private:
    ClienteRecepcionista *clienteInterno; // Subsistema encapsulado
};
```

**Responsabilidades:**
- Encapsula `ClienteRecepcionista` (subsistema complejo)
- Proporciona métodos simples como `enviarNuevoPedido()` en lugar de exponer detalles de protocolo TCP/JSON
- Reenvía señales del subsistema interno de forma controlada

#### 2. **VentanaRecepcionista** - El punto de inyección

```cpp
VentanaRecepcionista::VentanaRecepcionista(QWidget *parent) {
    // ✅ Crear UNA SOLA instancia del Facade
    facade = new RecepcionistaFacade(this);
    facade->conectarAlServidor("127.0.0.1", 5555);

    // ✅ Inyectar el Facade a los componentes que lo necesiten
    panelPedido = new PanelPedido(facade, this);
}
```

#### 3. **PanelPedido** - El cliente del Facade

```cpp
class PanelPedido : public QWidget {
public:
    // ✅ Recibe el Facade por inyección (no lo crea)
    explicit PanelPedido(RecepcionistaFacade *facade, QWidget *parent);

private:
    RecepcionistaFacade *facade; // ❌ No instancia, solo usa
};

void PanelPedido::enviarPedido() {
    // ✅ Llamada simple, sin conocer detalles de TCP/JSON
    facade->enviarNuevoPedido(mesaActual, idRecep, platosJson);
}
```

### Beneficios Obtenidos

| Aspecto | Antes | Después (Facade) |
|---------|-------|------------------|
| **Conexiones TCP** | Una por componente | Una única compartida |
| **Acoplamiento** | Alto (UI conoce TCP) | Bajo (UI usa interfaz simple) |
| **Testabilidad** | Difícil (requiere servidor real) | Fácil (mock del Facade) |
| **Mantenimiento** | Cambios en protocolo afectan toda la UI | Cambios aislados en Facade |
| **Responsabilidad** | UI maneja red y presentación | Separación clara de concerns |

### Diagrama de Flujo

```
┌─────────────────────────┐
│ VentanaRecepcionista    │ ← Crea y gestiona el Facade
└───────────┬─────────────┘
            │
            │ (inyecta)
            ▼
┌─────────────────────────┐
│ RecepcionistaFacade     │ ← Interfaz simplificada
│ ├─ conectarAlServidor() │
│ ├─ enviarNuevoPedido()  │
│ └─ obtenerMenu()        │
└───────────┬─────────────┘
            │
            │ (delega)
            ▼
┌─────────────────────────┐
│ ClienteRecepcionista    │ ← Subsistema complejo (TCP/JSON)
│ (oculto de la UI)       │
└─────────────────────────┘
```

## 🔧 Compilación

Requiere Qt 6 con módulos `Widgets` y `Network`:

```cmd
cd clients\recepcionista
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.6.3\msvc2022_64"
cmake --build build --config Release
```

Ejecutable: `build\Release\ClienteRecepcionista.exe`

## 📚 Otros Patrones Aplicados

- **Inyección de Dependencias**: `VentanaRecepcionista` inyecta `RecepcionistaFacade` en `PanelPedido`
- **Observer (Signals/Slots)**: Comunicación entre componentes mediante señales Qt
- **MVC**: Separación entre UI (`PanelPedido`), datos (`GestorPedidos`) y lógica de negocio (servidor)

## 🔗 Conexión con el Servidor

- **Host**: `127.0.0.1`
- **Puerto**: `5555`
- **Protocolo**: TCP con mensajes JSON
- **Comandos**: `SOLICITAR_MENU`, `NUEVO_PEDIDO`

---

**Desarrollado con Qt 6 y C++17**
