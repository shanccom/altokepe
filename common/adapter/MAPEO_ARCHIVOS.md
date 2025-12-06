# Mapeo de Archivos: Patrón Adapter para Serialización

## Directorio NUEVO (con Patrón Adapter)

**Ubicación**: `common/adapter/`

### Archivos Implementados

| Archivo | Rol en el Patrón | Descripción |
|---------|------------------|-------------|
| `ISerializador.h` | **Target (Interface)** | Interfaz genérica que define el contrato de serialización |
| `AdaptadorSerializadorJSON.h` | **Adapter (Header)** | Declaración del adaptador que implementa ISerializador |
| `AdaptadorSerializadorJSON.cpp` | **Adapter (Implementation)** | Implementación que delega a SerializadorJSON |

### Uso del Nuevo Patrón

```cpp
#include "common/adapter/AdaptadorSerializadorJSON.h"

// Crear instancia del adaptador
AdaptadorSerializadorJSON adaptador;

// Serializar
PlatoDefinicion plato = /* ... */;
QJsonObject json = adaptador.platoDefinicionToJson(plato);

// Deserializar
PlatoDefinicion platoRecuperado = adaptador.jsonToPlatoDefinicion(json);
```

### Ventajas

- ✅ Desacoplamiento del formato de serialización
- ✅ Facilita cambios futuros (XML, Protobuf, etc.)
- ✅ Interfaz uniforme para diferentes formatos
- ✅ Principio de Inversión de Dependencias (DIP)

---

## Directorio ANTIGUO (Implementación Directa - Legacy)

**Ubicación**: `common/network/`

### Archivos Existentes

| Archivo | Descripción |
|---------|-------------|
| `SerializadorJSON.h` | **Adaptee** - Clase concreta de serialización JSON (sin cambios) |
| `SerializadorJSON.cpp` | **Adaptee** - Implementación de serialización JSON (sin cambios) |

### Uso Antiguo (Aún Funcional)

```cpp
#include "common/network/SerializadorJSON.h"

// Uso directo (estático)
PlatoDefinicion plato = /* ... */;
QJsonObject json = SerializadorJSON::platoDefinicionToJson(plato);

// Deserializar
PlatoDefinicion platoRecuperado = SerializadorJSON::jsonToPlatoDefinicion(json);
```

### Archivos que Actualmente Usan el Método Antiguo

**Server:**
- `server/LogicaNegocio.cpp` (línea 5, 49)

**Clients:**
- `clients/cocina/manager_chef/network/ClienteManagerApp.cpp` (líneas 5, 51, 61, 72)
- `clients/cocina/manager_chef/ui/VentanaManager.cpp` (líneas 4, 45)

**CMake:**
- `clients/recepcionista/CMakeLists.txt` (línea 35)
- `clients/cocina/manager_chef/CMakeLists.txt` (línea 35)

> **Nota**: Estos archivos continúan funcionando sin modificaciones. La migración al nuevo patrón es opcional y gradual.

---

## Compatibilidad

- ✅ **100% Compatible**: El código antiguo sigue funcionando sin cambios
- ✅ **Sin Breaking Changes**: SerializadorJSON no fue modificado
- ✅ **Migración Gradual**: Se puede migrar archivo por archivo cuando sea conveniente
- ✅ **Mismo Comportamiento**: El adaptador produce exactamente el mismo JSON

---

## Migración Futura (Opcional)

Para migrar un archivo del método antiguo al nuevo:

### Paso 1: Cambiar el include
```cpp
// Antes
#include "common/network/SerializadorJSON.h"

// Después
#include "common/adapter/AdaptadorSerializadorJSON.h"
```

### Paso 2: Crear instancia del adaptador
```cpp
// Agregar al inicio de la clase o función
AdaptadorSerializadorJSON adaptador;
```

### Paso 3: Cambiar llamadas estáticas por llamadas de instancia
```cpp
// Antes
PlatoDefinicion plato = SerializadorJSON::jsonToPlatoDefinicion(json);

// Después
PlatoDefinicion plato = adaptador.jsonToPlatoDefinicion(json);
```

### Paso 4: Actualizar CMakeLists.txt
```cmake
# Agregar el nuevo archivo .cpp
common/adapter/AdaptadorSerializadorJSON.cpp
```
