# NLX - Network Linux Extension - Documentación del Proyecto

## 📋 Resumen del Proyecto

**NLX** es una herramienta de monitoreo de red en tiempo real con interfaz TUI (Terminal User Interface), diseñada para ser portable y eficiente. Es el proyecto hermano de GLX (GPU Linux Extension), pero enfocado en análisis de red. 

## 🎯 Objetivo Principal

Crear un monitor de red estilo `btop` pero especializado en análisis de red, que muestre:
- Ancho de banda en tiempo real (subida/bajada)
- Conexiones activas
- Métricas de latencia
- Alertas de red
- Información de procesos de red

## 🏗️ Arquitectura del Proyecto

### Estructura de Directorios
```
NLX/
├── src/                    # Código fuente
│   ├── main.c             # Punto de entrada, parsing de argumentos
│   ├── ui.c               # Interfaz TUI con ncurses
│   ├── collector.c        # Recolección de métricas de red
│   ├── analyzer.c         # Análisis de datos y alertas
│   ├── renderer.c         # Renderizado de gráficos ASCII
│   └── utils.c            # Funciones utilitarias
├── include/               # Headers
│   ├── ui.h               # Definiciones de interfaz TUI
│   ├── collector.h        # API de recolección de métricas
│   ├── analyzer.h         # Funciones de análisis
│   ├── renderer.h         # Funciones de renderizado
│   └── utils.h            # Utilidades comunes
├── config/                # Configuración
│   └── nlx.conf           # Archivo de configuración
├── build/                 # Ejecutables compilados
├── Makefile               # Compilación
├── install.sh             # Script de instalación
├── uninstall.sh           # Script de desinstalación
├── .gitignore             # Archivos a ignorar
├── LICENSE                # Licencia MIT
├── README.md              # Documentación pública
└── PROJECT_DOCS.md        # Esta documentación interna
```

### Flujo de Datos
```
main.c (loop principal)
    ↓
collector.c (recolecta métricas cada X segundos)
    ↓
analyzer.c (procesa y detecta alertas)
    ↓
renderer.c (prepara gráficos ASCII)
    ↓
ui.c (dibuja en pantalla con ncurses)
    ↓
[espera refresh_rate] → vuelve al collector
```

## 🔧 Tecnologías y Dependencias

### Bibliotecas Principales
- **ncurses** - Interfaz TUI
- **libpcap** - Captura de paquetes de red
- **libcurl** - Peticiones HTTP para pruebas de latencia
- **pthread** - Multithreading para operaciones concurrentes

### Comandos del Sistema Utilizados
- `/proc/net/dev` - Estadísticas de interfaces de red
- `/proc/net/tcp` - Conexiones TCP activas
- `/proc/net/udp` - Conexiones UDP activas
- `/proc/[pid]/fd/` - Descriptores de archivo de procesos
- `ping` - Pruebas de latencia

## 📊 Funcionalidades Planificadas

### 1. Monitoreo de Ancho de Banda
- **Fuentes**: `/proc/net/dev`, `/sys/class/net/`
- **Métricas**: bytes enviados/recibidos, paquetes, velocidades
- **Visualización**: Gráficos ASCII en tiempo real
- **Cálculo**: Velocidades de subida/bajada en MB/s

### 2. Conexiones Activas
- **Fuentes**: `/proc/net/tcp`, `/proc/net/udp`
- **Información**: IPs, puertos, estados, procesos
- **Filtros**: Por puerto, proceso, estado
- **Visualización**: Tabla con top conexiones

### 3. Métricas de Latencia
- **Servidores**: Google, GitHub, Cloudflare (configurables)
- **Método**: Ping con timeout configurable
- **Visualización**: Barras de progreso con colores
- **Alertas**: Cuando la latencia supera umbrales

### 4. Sistema de Alertas
- **Tipos**: Alto uso de ancho de banda, puertos sospechosos, latencia alta
- **Severidad**: low, medium, high, critical
- **Acciones**: Notificaciones en pantalla
- **Configuración**: Umbrales personalizables

### 5. Información de Procesos
- **Monitoreo**: Procesos que usan red
- **Métricas**: Bytes por proceso, conexiones por proceso
- **Visualización**: Lista ordenada por uso de red

## 🎮 Comandos de Usuario

### Comandos Principales
```bash
nx                    # Dashboard principal (modo TUI)
nx help               # Mostrar ayuda
nx status             # Estado actual del sistema
nx connections        # Solo conexiones activas
nx bandwidth          # Solo monitoreo de ancho de banda
nx latency            # Solo métricas de latencia
nx alerts             # Alertas y notificaciones
nx processes          # Procesos que usan red
```

### Modos de Interfaz
- **Dashboard**: Vista completa con todas las métricas
- **Conexiones**: Enfocado en conexiones activas
- **Ancho de banda**: Uso de ancho de banda y gráficos
- **Latencia**: Métricas de conectividad y ping
- **Alertas**: Alertas actuales
- **Procesos**: Procesos que usan red

## ⚙️ Configuración

### Archivo de Configuración (`nlx.conf`)
```ini
[general]
refresh_rate = 2              # Segundos entre actualizaciones
interface = auto              # Interfaz de red a monitorear

[latency]
servers = google.com,github.com,cloudflare.com
timeout = 5000                # Timeout en milisegundos

[alerts]
bandwidth_threshold = 90      # Porcentaje para alertas de ancho de banda
suspicious_ports = 22,23,3389 # Puertos considerados sospechosos
max_connections = 1000        # Máximo de conexiones antes de alerta
```

## 🎨 Interfaz de Usuario

### Dashboard Principal (TUI)
```
┌─ NLX v1.0 ──────────────────────────────────────────────────┐
│                                                             │
│  Ancho de Banda (Últimos 60s)                               │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │ ████████████████████████████████████████████████████████ │ │
│  │ ████████████████████████████████████████████████████████ │ │
│  │ ████████████████████████████████████████████████████████ │ │
│  └─────────────────────────────────────────────────────────┘ │
│  Descarga: 2.5 MB/s  │  Subida: 1.2 MB/s  │  Total: 3.7 MB/s │
│                                                             │
│  Conexiones Activas (Top 10)                                │
│  ┌─ Puerto ─┬─ Estado ─┬─ Proceso ─┬─ Dirección ─┬─ Bytes ─┐ │
│  │   80     │  LISTEN  │  nginx    │  0.0.0.0    │  2.1M  │ │
│  │  443     │  LISTEN  │  nginx    │  0.0.0.0    │  1.8M  │ │
│  │ 8080     │  ESTAB   │  node     │192.168.1.5  │  950K  │ │
│  └──────────┴──────────┴───────────┴─────────────┴─────────┘ │
│                                                             │
│  Latencia y Conectividad                                    │
│  Google: 15ms ●  │  GitHub: 45ms ●  │  Cloudflare: 8ms ●    │
│                                                             │
│  Alertas (2)                                                │
│  • Alto uso de ancho de banda (95%) - hace 30s              │
│  • Conexión sospechosa en puerto 22 - hace 2m               │
│                                                             │
│  [Q] Salir  [R] Actualizar  [1] Conexiones  [2] Ancho de banda │
│  [3] Latencia  [4] Alertas  [5] Procesos  [C] Config      │
└─────────────────────────────────────────────────────────────┘
```

## 🔄 Estados de Desarrollo

### ✅ Completado
- [x] Estructura básica del proyecto
- [x] Makefile funcional
- [x] Scripts de instalación/desinstalación (estructura)
- [x] Headers básicos
- [x] Compilación sin errores
- [x] Ejecutable básico funcionando
- [x] Parsing de argumentos en main.c
- [x] Interfaz TUI con ncurses
- [x] Recolección de métricas de red
- [x] Implementación de utils.c
- [x] Implementación de collector.c

### 🚧 En Progreso
- [ ] Sistema de análisis y alertas

### 📋 Pendiente
- [ ] Renderizado de gráficos ASCII
- [ ] Sistema de configuración
- [ ] Pruebas de latencia
- [ ] Información detallada de procesos
- [ ] Pruebas y debugging

## 🛠️ Comandos de Desarrollo

### Compilación
```bash
make              # Compilación normal
make debug        # Compilación con debug
make clean        # Limpiar build
```

### Pruebas
```bash
./build/nx        # Ejecutar versión de desarrollo
./build/nx help   # Probar comando help
./build/nx tui    # Probar interfaz TUI
```

### Instalación
```bash
make install      # Compilar e instalar
make uninstall    # Desinstalar
```

## 🔍 Diferencias con GLX

### GLX (GPU Linux Extension)
- **Propósito**: Control de configuración de GPU
- **Arquitectura**: Lexer → Parser → Interpreter
- **Funcionalidad**: Ejecutar archivos .gx con configuraciones
- **Comandos**: `gx run mode:performance`, `gx status`

### NLX (Network Linux Extension)
- **Propósito**: Monitoreo de red en tiempo real
- **Arquitectura**: Collector → Analyzer → UI
- **Funcionalidad**: Monitor continuo con interfaz TUI
- **Comandos**: `nx`, `nx connections`, `nx bandwidth`

## 📝 Notas de Implementación

### Consideraciones Técnicas
1. **Thread-safe**: Las actualizaciones deben ser thread-safe
2. **Eficiencia**: Mínimo uso de CPU y memoria
3. **Portabilidad**: Debe funcionar en diferentes distribuciones Linux
4. **Permisos**: Algunas operaciones requieren privilegios

### Desafíos Identificados
1. **Captura de paquetes**: Requiere permisos especiales
2. **Interfaz TUI**: Manejo de eventos y refresco
3. **Configuración**: Parsing de archivo INI

### Decisiones de Diseño
1. **ncurses**: Biblioteca estándar para TUI
2. **Modularidad**: Cada componente es independiente
3. **Configuración**: Archivo INI simple
4. **Tiempo real**: Sin almacenamiento persistente

## 🎯 Próximos Pasos

1. **Implementar sistema de alertas** en analyzer.c
2. **Desarrollar gráficos ASCII** en renderer.c
3. **Sistema de configuración** con nlx.conf
4. **Pruebas de latencia** funcionales
5. **Mejorar información de procesos**

## 📚 Referencias

- **GLX**: Proyecto hermano para control de GPU
- **btop**: Inspiración para diseño TUI
- **iftop**: Inspiración para métricas de red
- **htop**: Referencia para interfaz de terminal
- **nethogs**: Monitoreo de procesos de red

---

**Última actualización**: [Fecha actual]
**Versión del proyecto**: 0.2.0 (TUI funcional)
**Estado**: Interfaz TUI implementada 