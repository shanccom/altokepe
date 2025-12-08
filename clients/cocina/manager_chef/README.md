## 1. Introducción

El módulo **Manager Chef Client**, es una aplicación de escritorio desarrollada en C++ utilizando el framework Qt6. Este cliente forma parte del ecosistema de gestión de restaurantes "AltokePe" y su función principal es permitir al jefe de cocina visualizar el estado de los pedidos (pendientes, en progreso, terminados) y gestionar el flujo de preparación de los platos.

El cliente actúa como una interfaz gráfica que se conecta vía TCP al servidor central, recibiendo actualizaciones en tiempo real y enviando comandos de control sobre los pedidos.

## 2. Arquitectura y Diseño del Sistema

La aplicación sigue una arquitectura basada en **Capas (Layered Architecture)**, separando claramente la responsabilidad de la interfaz de usuario, la lógica de negocio y la comunicación de red. El diseño se apoya fuertemente en el sistema de **Señales y Slots** de Qt para mantener el desacoplamiento entre componentes.

### 2.1. Estructura de Componentes

1.  **Capa de Presentación (UI):**
    Encargada de dibujar la interfaz. Se ha modularizado utilizando un enfoque de composición:
    *   `VentanaManager`: La ventana principal que orquesta las columnas.
    *   `ColumnaPedidos`: Contenedores verticales que representan los estados (Pendiente, En Progreso, Terminado).
    *   `TarjetaPedido` y `TarjetaPlato`: Widgets personalizados que representan la información visual de cada entidad.

2.  **Capa de Lógica de Aplicación (Controller):**
    Representada por la clase `ClienteManagerApp`. Esta clase actúa como intermediario (Controller) entre la red y la interfaz.
    *   Recibe los eventos de red, actualiza su modelo interno de datos y decide qué métodos de la UI invocar.
    *   Recibe interacciones del usuario desde la UI y las transforma en comandos de red.

3.  **Capa de Red:**
    Gestionada por `ClienteTCP`. Abstrae la complejidad de los sockets y el protocolo de bytes, exponiendo una interfaz limpia basada en objetos JSON.

### 2.2. Diagrama de Flujo de Datos

El flujo es bidireccional y asíncrono:
*   **Server a UI:** `QTcpSocket` recibe bytes, `ClienteTCP` ensambla JSON, `ClienteManagerApp` interpreta evento y `VentanaManager` actualiza widgets.
*   **UI a Server:** Usuario hace clic en botón, `TarjetaPedido` emite señal, `ClienteManagerApp` construye comando JSON y `ClienteTCP` envía datos.

## 3. Patrones de Diseño

### 3.1. Patrón Observer (Señales y Slots)
Es el patrón fundamental de la aplicación. En lugar de usar punteros directos para las devoluciones de llamada (callbacks), los componentes emiten "señales" sin saber quién las recibirá.

Ejemplo en `TarjetaPedido.cpp`:
```cpp
// La tarjeta emite una señal, no llama directamente a la lógica
connect(m_btnPreparar, &QPushButton::clicked, [this](){ 
  emit prepararPedido(m_idPedido); 
});
```

> [!CAUTION]
> El patrón explicado no ha sido implementado desde cero ya que QT trae este patrón listo para ser usado. Sin embargo, solo se especifica para que se entienda la base en la que está construido todo el proyecto, que implementa aún más patrones de diseño desde cero.

### 3.2. Patrón Composite
Utilizado en la construcción de la interfaz gráfica. La `VentanaManager` contiene `ColumnaPedidos`, que a su vez contiene una lista de `TarjetaPedido`, la cual contiene múltiples `TarjetaPlato`. Esto permite tratar a grupos de objetos de la misma manera que a un objeto individual (por ejemplo, al aplicar estilos o layouts).

### 3.3. Patrón Facade (Fachada)
La clase `ClienteTCP` actúa como una fachada para `QTcpSocket`. Oculta la complejidad de la gestión de buffers, la conexión asíncrona y la conversión de `QByteArray` a `QJsonObject`, proporcionando métodos simples como `conectar()` y `enviarMensaje()`.

## 4. Mecanismo de Comunicación Cliente-Servidor

La comunicación se realiza mediante sockets TCP/IP estándar. El protocolo de aplicación se basa en el intercambio de mensajes serializados en formato JSON.

### 4.1. Protocolo de Delimitación
Dado que TCP es un protocolo de flujo (stream) y no de mensajes, los paquetes pueden llegar fragmentados o concatenados. Para solucionar esto, el cliente implementa un mecanismo de delimitación por salto de línea (`\n`).

En `ClienteTCP::listoParaLeer()`:
```cpp
// Se acumulan los bytes en un buffer persistente
m_buffer.append(m_socket->readAll());

// Se procesa mientras existan delimitadores
while (m_buffer.contains('\n')) {
  int newlinePos = m_buffer.indexOf('\n');
  QByteArray jsonData = m_buffer.left(newlinePos);
  m_buffer.remove(0, newlinePos + 1); // Se limpia lo procesado
  
  // Se deserializa el mensaje completo
  QJsonDocument doc = QJsonDocument::fromJson(jsonData);
  emit nuevoMensajeRecibido(doc.object());
}
```

### 4.2. Estructura del Mensaje JSON
Los mensajes siguen una estructura de diccionario clave-valor definida en `Protocolo.h`.
*   **Envío (Comandos):** Incluyen una clave `comando` (por ejemplo, `PREPARAR_PEDIDO`) y un objeto `data` con los parámetros.
*   **Recepción (Eventos):** Incluyen una clave `evento` (por ejemplo, `PEDIDO_REGISTRADO`) y un objeto `data`.

## 5. Manejo de Errores y Excepciones

En este proyecto, se ha optado por un enfoque de **programación defensiva y manejo de errores basado en estados**, en lugar del uso extensivo de bloques `try-catch` estándar de C++. Esto es común en aplicaciones Qt debido a su naturaleza orientada a eventos.

### 5.1. Validación de Datos JSON
Antes de procesar un mensaje, se verifica la integridad del documento JSON utilizando las herramientas de Qt. Si el JSON está mal formado, el mensaje se descarta y se registra una advertencia, evitando que la aplicación colapse.

```cpp
if (!doc.isNull() && doc.isObject()) {
  emit nuevoMensajeRecibido(doc.object());
} else {
  qWarning() << "Mensaje JSON inválido recibido:" << QString(jsonData);
}
```

### 5.2. Estado de la Conexión
Antes de intentar escribir en el socket, se verifica explícitamente si la conexión está activa para evitar errores de escritura en un descriptor de archivo cerrado.

```cpp
if (m_socket->state() == QAbstractSocket::ConnectedState) {
  m_socket->write(...);
} else {
  qWarning() << "No se puede enviar mensaje, socket no conectado.";
}
```

### 5.3. Recursos Gráficos
La carga de la hoja de estilos (`styles.qss`) incluye una verificación de apertura del archivo. Si falla, la aplicación continúa con el estilo por defecto, degradándose suavemente en lugar de terminar la ejecución.

## 6. Separación de Estilos y Lógica

Se ha utilizado un archivo `.qss` (Qt Style Sheets) externo para definir la apariencia visual. Esto separa la lógica de C++ del diseño visual (colores, bordes, fuentes), permitiendo modificar la estética sin recompilar el código fuente.

El uso de **Propiedades Dinámicas** es notable en `TarjetaPlato`. Se asigna una propiedad personalizada (`estado_plato`) que el archivo QSS utiliza como selector para cambiar el color de fondo automáticamente según el estado del plato (por ejemplo, Verde para "FINALIZADO", Naranja para "EN_PROGRESO").

```css
/* Ejemplo en styles.qss */
QLabel[estado_plato="FINALIZADO"] { background-color: #5cb85c; }
```
