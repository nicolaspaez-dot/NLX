# NLX - Network Linux Extension

Una herramienta de monitoreo de red en tiempo real con interfaz TUI, diseñada para ser portable y eficiente como GLX.

## Descripción General

NLX proporciona capacidades completas de monitoreo de red a través de una interfaz de terminal intuitiva. Muestra el uso de ancho de banda, conexiones activas, métricas de latencia y alertas de red en tiempo real, similar a herramientas como btop pero enfocada en análisis de red.

## Características

### Funcionalidad Principal
- **Monitoreo de ancho de banda en tiempo real** - Velocidades de descarga/subida con gráficos ASCII
- **Visualización de conexiones activas** - Puertos, procesos, direcciones IP y transferencia de datos
- **Monitoreo de latencia** - Ping a servicios populares (Google, GitHub, Cloudflare)
- **Alertas de red** - Detección automática de uso alto y conexiones sospechosas
- **Datos históricos** - Almacenamiento basado en SQLite para análisis de tendencias
- **Exportación de datos** - Formatos CSV y JSON para análisis externo

### Modos de Interfaz
- **Modo dashboard** (`nx`) - Vista completa con todas las métricas
- **Modo conexiones** (`nx connections`) - Enfocado en conexiones activas
- **Modo ancho de banda** (`nx bandwidth`) - Uso de ancho de banda y gráficos
- **Modo latencia** (`nx latency`) - Métricas de conectividad y ping
- **Modo alertas** (`nx alerts`) - Alertas actuales e historial
- **Modo historial** (`nx history`) - Datos históricos y tendencias

### Configuración
- **Tasas de actualización personalizables** - Intervalos de 1s, 2s, 5s
- **Servidores de latencia configurables** - Agregar/quitar objetivos de ping
- **Umbrales de alertas** - Establecer límites personalizados para notificaciones
- **Selección de interfaz** - Elegir qué interfaz de red monitorear
- **Retención de datos** - Configurar cuánto tiempo mantener datos históricos

## Arquitectura

```
NLX/
├── src/
│   ├── main.c              # Punto de entrada y parsing de comandos
│   ├── ui.c                # Interfaz TUI con ncurses
│   ├── collector.c         # Recolección de métricas de red
│   ├── analyzer.c          # Análisis de datos y alertas
│   ├── storage.c           # Operaciones de base de datos SQLite
│   └── utils.c             # Funciones utilitarias
├── include/
│   ├── ui.h                # Definiciones de interfaz TUI
│   ├── collector.h         # API de recolección de métricas
│   ├── analyzer.h          # Funciones de análisis
│   ├── storage.h           # Operaciones de base de datos
│   └── utils.h             # Utilidades comunes
├── config/
│   └── nlx.conf            # Archivo de configuración
├── data/                   # Archivos de base de datos SQLite
├── install.sh              # Script de instalación global
├── uninstall.sh            # Script de desinstalación
└── Makefile
```

## Requisitos del Sistema

### Dependencias
- **ncurses** - Biblioteca de interfaz de terminal
- **libpcap** - Biblioteca de captura de paquetes
- **SQLite3** - Almacenamiento de base de datos
- **libcurl** - Peticiones HTTP para pruebas de latencia

### Permisos
- **CAP_NET_RAW** - Para captura de paquetes
- **CAP_NET_ADMIN** - Para acceso a interfaces de red
- **Acceso sudo** - Para operaciones privilegiadas

## Instalación

```bash
# Clonar el repositorio
git clone <url-del-repositorio>
cd NLX

# Instalar dependencias (Arch Linux)
sudo pacman -S ncurses libpcap sqlite3 libcurl

# Compilar e instalar
make
sudo ./install.sh
```

## Uso

### Comandos Básicos
```bash
nx                    # Dashboard principal
nx connections        # Solo conexiones activas
nx bandwidth          # Solo monitoreo de ancho de banda
nx latency            # Solo métricas de latencia
nx alerts             # Alertas y notificaciones
nx history            # Datos históricos
nx help               # Mostrar ayuda
```

### Interfaz del Dashboard
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
│  [3] Latencia  [4] Alertas  [5] Historial  [C] Config      │
└─────────────────────────────────────────────────────────────┘
```

## Configuración

### Archivo de Configuración Principal (`/usr/local/share/nlx/nlx.conf`)
```ini
[general]
refresh_rate = 2
data_retention_days = 30
interface = auto

[latency]
servers = google.com,github.com,cloudflare.com
timeout = 5000

[alerts]
bandwidth_threshold = 90
suspicious_ports = 22,23,3389
max_connections = 1000

[export]
default_format = csv
auto_export = false
export_path = /tmp/nlx_exports
```

## Exportación de Datos

### Formatos Soportados
- **CSV** - Valores separados por comas para análisis en hojas de cálculo
- **JSON** - Datos estructurados para integración con APIs

### Comandos de Exportación
```bash
nx export bandwidth --format csv --period 24h
nx export connections --format json --filter port=80
nx export alerts --format csv --since 2024-01-01
```

## Desarrollo

### Compilación desde Fuente
```bash
make clean
make
```

### Ejecutar Pruebas
```bash
make test
```

### Estructura del Código
- **Diseño modular** - Cada componente es autónomo
- **Thread-safe** - Actualizaciones en tiempo real sin bloquear la UI
- **Eficiente en memoria** - Uso mínimo de recursos
- **Manejo de errores** - Degradación elegante en fallos

## Contribución

1. Haz fork del repositorio
2. Crea una rama de características
3. Realiza tus cambios
4. Agrega pruebas si es aplicable
5. Envía un pull request

## Licencia

Este proyecto está licenciado bajo la Licencia MIT - ver el archivo LICENSE para detalles.

## Proyectos Relacionados

- **GLX** - GPU Linux Extension (proyecto hermano)
- **btop** - Monitor de recursos (inspiración para diseño TUI)
- **iftop** - Monitoreo de red (inspiración para métricas)

## Hoja de Ruta

### Versión 1.1
- Sistema de plugins para métricas personalizadas
- API web para integración externa
- Filtrado y búsqueda avanzada

### Versión 1.2
- Monitoreo multi-interfaz
- Acciones de alerta personalizadas
- Optimizaciones de rendimiento

### Versión 2.0
- Monitoreo distribuido
- Machine learning para detección de anomalías
- Dashboard web 