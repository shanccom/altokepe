## 1. Introducción

El módulo **Manager Estación** es una aplicación de escritorio en C++/Qt6 que permite a cada estación de cocina visualizar y despachar los platos que le corresponden. Muestra una lista priorizada de platos, permite marcarlos como listos y refleja los cambios recibidos en tiempo real desde el servidor AltokePe.

## 2. Arquitectura y Diseño del Sistema

La app sigue una arquitectura por capas, apoyándose en el sistema de **Señales y Slots** de Qt para mantener bajo acoplamiento entre UI, lógica y red.

### 2.1. Estructura de Componentes

1. **Capa de Presentación (UI):**
   * `VentanaEstacion`: tabla de platos priorizados y botón de finalización.
   * `VentanaEstacionesUnificadas` (si se usa): orquesta múltiples estaciones en una sola ventana.

2. **Capa de Lógica de Aplicación (Controller):**
   * `ClienteEstacionApp`: interpreta eventos de red y actualiza la UI; convierte acciones del usuario en comandos.

3. **Capa de Red:**
   * `ClienteTCP`: encapsula el socket Qt y el protocolo de mensajes JSON delimitados por `\n`.

### 2.2. Flujo de Datos

* **Servidor → UI:** `QTcpSocket` recibe bytes → `ClienteTCP` ensambla JSON → `ClienteEstacionApp` interpreta evento → `VentanaEstacion` actualiza filas/estados.
* **UI → Servidor:** Usuario pulsa "Finalizar" → `VentanaEstacion` emite señal → `ClienteEstacionApp` crea comando JSON → `ClienteTCP` envía por TCP.

## 3. Patrones de Diseño

### 3.1. Patrón Observer (Señales y Slots)
Base de la comunicación interna. La UI emite señales sin conocer quién las procesa.

```cpp
// VentanaEstacion.cpp
connect(btnFinalizar, &QPushButton::clicked, this, &VentanaEstacion::onFinalizarMayorPrioridad);
// ... y más adelante
emit marcarListoSolicitado(idPedido, idInstancia);
```

> [!CAUTION]
> El patrón Observer lo provee Qt; no se implementa manualmente, pero es el fundamento para desacoplar componentes.

### 3.2. Patrón Composite
La ventana principal compone widgets (tabla + botón), y en escenarios con `VentanaEstacionesUnificadas` se anidan varias `VentanaEstacion`, tratándolas como unidades equivalentes para distribuir layout y estilo.

### 3.3. Patrón Facade (Fachada)
`ClienteTCP` actúa como fachada de `QTcpSocket`, ocultando buffers, reconexiones y parsing JSON, exponiendo métodos simples como `conectar()` y `enviarMensaje()`.

## 4. Mecanismo de Comunicación Cliente-Servidor

### 4.1. Delimitación
TCP es un stream; los mensajes se separan por salto de línea (`\n`). El buffer se procesa mientras existan delimitadores.

### 4.2. Mensajes JSON
Claves definidas en `Protocolo.h`:
* **Comandos (envío):** `comando` + objeto `data` (ej.: `MARCAR_LISTO`, `ESTACION_READY`).
* **Eventos (recepción):** `evento` + objeto `data` (ej.: `PLATO_ASIGNADO`, `PEDIDO_ACTUALIZADO`).

## 5. Manejo de Errores y Robustez

* **Validación JSON:** Se descartan documentos mal formados antes de emitir señales.
* **Estado de conexión:** Se verifica `ConnectedState` antes de escribir en el socket; si no, se registra advertencia.
* **Operaciones en tabla:** Se comprueba existencia de filas/IDs antes de insertar o borrar para evitar índices inválidos.

## 6. Separación de Estilos

El archivo `styles.qss` concentra colores, bordes y tipografías. La lógica C++ permanece limpia, y los ajustes visuales no requieren recompilar.

## 7. Compilación Rápida

Requiere Qt 6 (`Widgets`, `Network`). Ejemplo con MSVC 2022:

```cmd
cd clients\cocina\manager_estacion
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.6.3\msvc2022_64"
cmake --build build --config Release
```

Ejecutable resultante:
- `build\Release\ManagerChefClient.exe` (nombre heredado del `CMakeLists.txt`).

## 8. Resumen de Responsabilidades

| Componente | Rol principal |
|------------|---------------|
| `VentanaEstacion` | Mostrar platos priorizados y enviar señal de finalización |
| `ClienteEstacionApp` | Orquestar la lógica entre red y UI |
| `ClienteTCP` | Gestionar socket TCP y mensajes JSON |
| `styles.qss` | Definir la apariencia visual |
