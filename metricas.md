# Métricas de NLX - Network Linux Extension

## Querido usuario,

Bienvenido a **NLX** (Network Linux Extension), tu herramienta de monitoreo de red en tiempo real. Este documento te explicará detalladamente todas las métricas y información que puedes ver en la interfaz, para que puedas aprovechar al máximo las capacidades de monitoreo de red.

---

## 📊 **Ancho de Banda en Tiempo Real**

### **¿Qué es?**
Esta sección muestra el uso de ancho de banda de tu red en tiempo real, con un gráfico visual que se actualiza constantemente.

### **¿Qué puedes ver?**

#### **Gráfico de Barras**
- **Barras verdes** (#): Uso normal de ancho de banda (0-50% del máximo)
- **Barras amarillas** (#): Uso moderado (50-80% del máximo)
- **Barras rojas** (#): Uso alto (80-100% del máximo)
- **Altura de las barras**: Representa la velocidad en ese momento
- **Ancho completo**: Se ajusta automáticamente al tamaño de tu terminal

#### **Valor Máximo**
- **"Max: X.X MB/s"**: La velocidad más alta registrada en la sesión actual
- **Se actualiza dinámicamente**: Aumenta si detecta velocidades más altas

### **¿Cómo interpretarlo?**
- **Barras bajas**: Poco tráfico de red
- **Barras altas**: Mucho tráfico (descargas, streaming, etc.)
- **Patrones**: Puedes ver picos de uso y períodos de calma

---

## 🔧 **Información de Interfaz**

### **¿Qué es?**
Detalles específicos de la interfaz de red que está siendo monitoreada (ej: wlan0, eth0, etc.).

### **¿Qué puedes ver?**

#### **Identificación de Interfaz**
- **"Interfaz: wlan0"**: Nombre de la interfaz activa
- **Selección automática**: NLX elige la interfaz con más tráfico

#### **Bytes Totales**
- **"Bytes recibidos: 1.3 GB"**: Total de datos descargados desde el inicio
- **"Bytes enviados: 246.1 MB"**: Total de datos subidos desde el inicio
- **Formato inteligente**: Se muestra en KB, MB, GB según el tamaño

#### **Paquetes de Red**
- **"Paquetes recibidos: 1,840,456"**: Número total de paquetes descargados
- **"Paquetes enviados: 996,794"**: Número total de paquetes subidos
- **Indicador de actividad**: Más paquetes = más actividad de red

#### **Velocidades Actuales**
- **"Velocidad de DESCARGA actual: 9.24 KB/s"**: Velocidad de descarga en tiempo real
- **"Velocidad de SUBIDA actual: 0.00 KB/s"**: Velocidad de subida en tiempo real
- **Actualización constante**: Se actualiza cada segundo
- **Colores destacados**: Verde para subida, azul para descarga

### **¿Cómo interpretarlo?**
- **Velocidades altas**: Actividad intensa (streaming, descargas, etc.)
- **Velocidades bajas**: Uso normal (navegación, emails, etc.)
- **Diferencia entre bytes y paquetes**: Paquetes pequeños pero numerosos vs. paquetes grandes

---

## 🌐 **Conexiones Activas**

### **¿Qué es?**
Lista de todas las conexiones de red activas en tu sistema, mostrando qué procesos están usando la red.

### **¿Qué puedes ver?**

#### **Tabla de Conexiones**
- **"Puerto"**: Puerto de la conexión (80, 443, 22, etc.)
- **"Estado"**: Estado de la conexión
  - **LISTEN**: Servidor esperando conexiones
  - **ESTAB**: Conexión establecida
  - **TIME_WAIT**: Conexión cerrando
  - **CLOSE_WAIT**: Conexión cerrada por el otro lado

#### **Información de Procesos**
- **"Proceso"**: Nombre del programa usando la conexión
  - **nginx**: Servidor web
  - **node**: Aplicación Node.js
  - **ssh**: Conexión SSH
  - **systemd-resolve**: Resolución DNS

#### **Direcciones y Datos**
- **"IP Remota"**: Dirección IP del otro extremo
  - **0.0.0.0**: Escuchando en todas las interfaces
  - **192.168.1.X**: Red local
  - **8.8.8.8**: Servidor DNS de Google
- **"Bytes"**: Cantidad de datos transferidos por esa conexión

#### **Estadísticas Totales**
- **"Total conexiones TCP: 13"**: Número total de conexiones activas
- **"Procesos activos: 367"**: Número total de procesos en el sistema

### **¿Cómo interpretarlo?**
- **Puertos bajos (1-1024)**: Servicios del sistema
- **Puertos altos (>1024)**: Aplicaciones de usuario
- **Muchas conexiones**: Sistema muy activo
- **Conexiones sospechosas**: Puertos inusuales o IPs desconocidas

---

## 🔌 **Interfaces de Red**

### **¿Qué es?**
Lista de todas las interfaces de red disponibles en tu sistema y su estado actual.

### **¿Qué puedes ver?**

#### **Estado de Interfaces**
- **"* wlan0 = activa"**: Interfaz activa (conectada y funcionando)
- **"o enp7s0 = inactiva"**: Interfaz inactiva (desconectada o apagada)
- **Símbolos**:
  - **"*"**: Interfaz activa
  - **"o"**: Interfaz inactiva

#### **Tipos de Interfaces**
- **wlan0**: Conexión WiFi
- **eth0/enp7s0**: Conexión Ethernet
- **lo**: Loopback (localhost)
- **docker0**: Red de Docker (si está instalado)

#### **Información Adicional**
- **"Interfaces encontradas: X"**: Número total de interfaces detectadas
- **"* = Activa o = Inactiva"**: Leyenda de símbolos

### **¿Cómo interpretarlo?**
- **Interfaz activa**: Está conectada y puede transferir datos
- **Interfaz inactiva**: No está en uso o no tiene conexión
- **Múltiples interfaces**: Sistema con WiFi y Ethernet
- **Solo una activa**: Normal en laptops o sistemas simples

---

## 🎮 **Comandos Disponibles**

### **Controles de la Interfaz**
- **[Q] Salir**: Cerrar NLX y volver al terminal
- **[R] Actualizar**: Refrescar los datos inmediatamente
- **[1] Ancho de banda**: Enfocarse solo en métricas de ancho de banda
- **[2] Conexiones**: Ver solo las conexiones activas
- **[3] Interfaces**: Mostrar solo información de interfaces

---

## 🎯 **Consejos de Uso**

### **Para Monitoreo General**
1. **Observa el gráfico**: Los patrones te dicen cuándo hay actividad
2. **Revisa las velocidades**: Identifica qué está usando más ancho de banda
3. **Monitorea conexiones**: Detecta aplicaciones que usan la red

### **Para Diagnóstico**
1. **Velocidades bajas**: Verifica tu conexión a internet
2. **Muchas conexiones**: Revisa si hay aplicaciones consumiendo recursos
3. **Interfaces inactivas**: Verifica cables o configuración WiFi

### **Para Seguridad**
1. **Conexiones sospechosas**: Revisa IPs y puertos desconocidos
2. **Procesos inusuales**: Identifica programas que no deberían usar la red
3. **Puertos altos**: Verifica aplicaciones que abren puertos

---

## 🔄 **Actualización de Datos**

### **Frecuencia**
- **Gráfico**: Se actualiza cada segundo
- **Velocidades**: Cálculo en tiempo real
- **Conexiones**: Refresco automático
- **Interfaces**: Verificación constante

### **Precisión**
- **Datos del kernel**: Información directa del sistema
- **Sin estimaciones**: Métricas reales de `/proc/net/`
- **Tiempo real**: Sin retrasos significativos

---

## 📈 **Interpretación de Valores**

### **Velocidades Típicas**
- **0-1 MB/s**: Navegación web, emails
- **1-10 MB/s**: Streaming de video, descargas pequeñas
- **10-100 MB/s**: Descargas grandes, backup
- **>100 MB/s**: Transferencias locales, servidores

### **Conexiones Típicas**
- **<50**: Uso normal
- **50-200**: Sistema activo
- **>200**: Mucha actividad o servidor

### **Puertos Comunes**
- **22**: SSH
- **80**: HTTP
- **443**: HTTPS
- **53**: DNS
- **8080**: Servidor de desarrollo

---

## 🎉 **¡Disfruta Monitoreando tu Red!**

NLX te proporciona una vista completa y en tiempo real de tu actividad de red. Úsalo para:
- **Optimizar tu conexión**
- **Detectar problemas**
- **Monitorear aplicaciones**
- **Aprender sobre redes**

**¡Gracias por usar NLX!** 🚀

---

*Documento generado automáticamente por NLX v1.0* 