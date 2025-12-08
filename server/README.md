# Servidor Altokepe

## Arquitectura

### Componentes Principales

**Servidor (`Servidor`)**
- Escucha conexiones TCP en el puerto 5555
- Crea un thread independiente para cada cliente conectado
- Gestiona comunicación asíncrona mediante signals/slots de Qt

**Manejador de Cliente (`ManejadorCliente`)**
- Gestiona la conexión individual de cada cliente
- Identifica el tipo de actor (Recepcionista, ManagerChef, EstacionCocina, Ranking)
- Procesa mensajes JSON línea por línea
- Utiliza patrón Command para ejecutar acciones de negocio

**Lógica de Negocio (`LogicaNegocio`)**
- Singleton que centraliza toda la lógica del sistema
- Thread-safe mediante mutex
- Gestiona el menú, pedidos activos y colas de preparación
- Coordina el flujo completo: desde nuevo pedido hasta entrega
- Aplica manejo robusto de errores mediante:
  - Validaciones estrictas de payload
  - Excepciones controladas (`ErrorLogica`)
  - Conversión de excepciones a mensajes JSON de error enviados al cliente
  - Mecanismos de recuperación segura para mantener coherencia del estado

## Flujo de Operación

### 1. Nuevo Pedido
```
Recepcionista → NUEVO_PEDIDO → LogicaNegocio
- Genera ID único de pedido
- Crea instancias de platos
- Estado: PENDIENTE
- Notifica a ManagerChef
```

### 2. Preparación
```
ManagerChef → PREPARAR_PEDIDO → LogicaNegocio
- Valida orden FIFO (cola)
- Cambia estado a EN_PROGRESO
- Distribuye platos a colas por estación
- Notifica a EstacionCocina
```

### 3. Cocción
```
EstacionCocina → MARCAR_PLATO_TERMINADO → LogicaNegocio
- Valida que sea el plato en preparación (top de cola)
- Estado: FINALIZADO
- Activa siguiente plato en cola
- Si todos terminaron: Pedido LISTO
```

### 4. Entrega
```
Recepcionista → CONFIRMAR_ENTREGA → LogicaNegocio
- Valida estado LISTO
- Marca pedido como ENTREGADO
- Actualiza ranking de platos
- Broadcast a pantallas Ranking
```

### 5. Devolución
```
ManagerChef → DEVOLVER_PLATO → LogicaNegocio
- Valida que esté FINALIZADO o ENTREGADO
- Re-encola con prioridad alta (score × 1.5)
- Actualiza ranking (decrementa)
- Notifica a cocina
```

## Características Clave

### Sistema de Colas con Prioridad
- Cada estación de cocina tiene su cola de prioridad
- Ordenamiento por `tiempo_preparacion_estimado`
- Platos devueltos tienen mayor prioridad

### Validaciones Estrictas
- **FIFO**: Solo se puede preparar el pedido al inicio de la cola
- **Orden de preparación**: Solo se puede terminar el plato actualmente en preparación
- **Estados coherentes**: Transiciones de estado validadas
- **Cancelación**: Solo si no está en preparación activa

### Thread-Safety
- Mutex protege todas las estructuras compartidas
- Comunicación entre threads mediante signals de Qt
- Cada cliente en su propio thread

### Notificaciones en Tiempo Real
- Broadcast selectivo por tipo de actor
- Actualizaciones automáticas de ranking
- Sincronización de estado entre todos los clientes

## Protocolo de Comunicación

Los mensajes siguen el formato JSON:
```json
{
  "evento": "NOMBRE_EVENTO",
  "data": { ... }
}
```

### Eventos Soportados
- `NUEVO_PEDIDO` - Crear pedido
- `PREPARAR_PEDIDO` - Iniciar preparación
- `MARCAR_PLATO_TERMINADO` - Finalizar plato
- `CONFIRMAR_ENTREGA` - Entregar pedido
- `DEVOLVER_PLATO` - Devolver plato
- `CANCELAR_PEDIDO` - Cancelar pedido
- `OTROS` - Posibles eventos adicionales

## Patrones de Diseño

- **Singleton**: LogicaNegocio
- **Command**: Procesamiento de comandos de negocio
- **Factory**: Creación de comandos e identificadores
- **Facade**: Interface simplificada para Ranking
- **Adapter**: Serialización JSON
- **Repository**: LogicaNegocio

