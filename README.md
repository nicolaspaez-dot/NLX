# NLX - Network Linux Extension

Una herramienta integral de monitoreo y análisis de red para sistemas Linux, que proporciona estadísticas de red en tiempo real, monitoreo de conexiones y métricas de rendimiento a través de una interfaz de usuario intuitiva en terminal.

## Descripción General

NLX es una potente aplicación de monitoreo de red basada en C que ofrece información detallada sobre el rendimiento de la red, conexiones activas y recursos del sistema. Construida con ncurses para un rendimiento óptimo en terminal, proporciona una experiencia de usuario fluida y sin parpadeo con visualización de datos en tiempo real.

## Características

### Monitoreo de Red
- Seguimiento del uso de ancho de banda en tiempo real
- Monitoreo del estado de interfaces
- Estadísticas de paquetes y bytes
- Visualización de velocidades de descarga/subida
- Gráficos históricos de ancho de banda

### Análisis de Conexiones
- Visualización de conexiones TCP/UDP activas
- Mapeo de procesos a conexiones
- Seguimiento de direcciones IP remotas
- Monitoreo del estado de conexiones
- Estadísticas de utilización de puertos

### Integración del Sistema
- Soporte para múltiples interfaces
- Seguimiento del uso de red por procesos
- Capacidades de prueba de latencia
- Detección de direcciones IP de interfaces
  

### Interfaz de Usuario
- Interfaz gráfica basada en terminal (TUI)
- Renderizado optimizado con doble buffer
- Información codificada por colores
- Actualizaciones de datos en tiempo real

### Instalación Automática (Recomendada)
```bash
# Clonar el repositorio
git clone https://github.com/nicolaspaez-dot/NLX.git
cd NLX

# Ejecutar instalador automático
./install.sh
```

El instalador automático:
- Detecta automáticamente tu distribución Linux
- Instala todas las dependencias necesarias
- Compila la aplicación
- Instala NLX en el sistema
- Crea enlaces simbólicos para acceso global


### Interfaz de Línea de Comandos
```bash
# Mostrar ayuda
nx help

# Mostrar estado del sistema
nx status

# Monitorear uso de ancho de banda
nx bandwidth

# Ver conexiones activas
nx connections

# Listar interfaces de red
nx interfaces

# Probar latencia de red
nx latency

# Mostrar procesos activos
nx processes
```

La TUI proporciona:
- Gráficos de ancho de banda en tiempo real
- Tablas de conexiones
- Estado de interfaces
- Estadísticas del sistema
- Navegación interactiva

### Controles de la TUI
- `Q` - Salir de la aplicación
- `R` - Actualizar datos
- `1` - Enfocar sección de ancho de banda
- `2` - Enfocar sección de conexiones
- `3` - Enfocar sección de interfaces

## Arquitectura

### Componentes Principales
- **Aplicación Principal** (`main.c`) - Punto de entrada y manejo de comandos
- **Interfaz de Usuario** (`ui.c`) - Implementación y renderizado de TUI
- **Recolector de Datos** (`collector.c`) - Recolección de estadísticas de red
- **Renderizador** (`renderer.c`) - Gráficos y visualización
- **Utilidades** (`utils.c`) - Funciones auxiliares y formateo

### Tecnologías Clave
- **ncurses** - Framework de interfaz de terminal
- **libpcap** - Captura de paquetes de red
- **libcurl** - Peticiones HTTP y pruebas de latencia
- **pthread** - Soporte para multi-hilos

## Optimizaciones de Rendimiento


### Gestión de Memoria
- Estructuras de datos eficientes para procesamiento en tiempo real
- Asignación y liberación adecuada de memoria
- Manejo y formateo optimizado de cadenas

## Desarrollo

### Estructura del Proyecto
```
NLX/
├── src/           # Código fuente
├── include/       # Archivos de cabecera
├── build/         # Binarios compilados
├── config/        # Archivos de configuración
├── data/          # Almacenamiento de datos
├── tests/         # Archivos de prueba
└── docs/          # Documentación
```

### Compilación para Desarrollo
```bash
# Compilación de debug con advertencias adicionales
make debug

# Limpiar compilación
make clean

# Ejecutar pruebas
make test
```


## Licencia

Este proyecto está licenciado bajo la Licencia MIT - ver el archivo [LICENSE](LICENSE) para más detalles.

## Contacto

Para preguntas, reportes de errores o contribuciones, por favor contacta:

**Email:** nico.paezdelgadillospr@gmail.com
---
