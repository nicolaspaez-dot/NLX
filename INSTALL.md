# Instalación de NLX

## Distribuciones Soportadas

NLX es compatible con las siguientes distribuciones Linux y sus derivadas:

### Debian/Ubuntu y derivadas
- **Debian** (todas las versiones)
- **Ubuntu** (todas las versiones)
- **Linux Mint**
- **Pop!_OS**
- **Elementary OS**
- **Kali Linux**
- **Parrot OS**

### Arch Linux y derivadas
- **Arch Linux**
- **Manjaro**
- **EndeavourOS**
- **Garuda Linux**
- **Artix Linux**

### Fedora y derivadas
- **Fedora** (todas las versiones)
- **Nobara**

### RHEL/CentOS y derivadas
- **Red Hat Enterprise Linux**
- **CentOS**
- **Rocky Linux**
- **AlmaLinux**
- **Oracle Linux**

### openSUSE
- **openSUSE Tumbleweed**
- **openSUSE Leap**

## Instalación Automática

### Requisitos previos
- Usuario con permisos sudo
- Conexión a internet para descargar dependencias

### Pasos de instalación

1. **Clonar o descargar el repositorio:**
   ```bash
   git clone https://github.com/tu-usuario/nlx.git
   cd nlx
   ```

2. **Ejecutar el instalador:**
   ```bash
   ./install.sh
   ```

3. **El script automáticamente:**
   - Detecta tu distribución Linux
   - Instala las dependencias necesarias
   - Compila NLX
   - Instala el ejecutable en `/usr/local/bin/nx`
   - Crea un enlace simbólico en `/usr/bin/nx`
   - Instala la documentación y configuración

## Instalación Manual

Si tu distribución no está soportada, puedes instalar manualmente:

### Dependencias requeridas
- **Compilador C:** `gcc` y `make`
- **Librerías de desarrollo:**
  - `ncurses-devel` (o `libncurses5-dev`)
  - `libpcap-devel` (o `libpcap-dev`)
  - `libcurl-devel` (o `libcurl4-openssl-dev`)
- **Herramientas de red:**
  - `iproute2` (o `iproute`)
  - `iputils` (o `iputils-ping`)

### Compilación manual
```bash
make clean
make
sudo cp build/nx /usr/local/bin/nx
sudo chmod +x /usr/local/bin/nx
sudo ln -sf /usr/local/bin/nx /usr/bin/nx
```

## Desinstalación

Para desinstalar NLX completamente:

```bash
./uninstall.sh
```

**Nota:** El script de desinstalación no elimina las dependencias del sistema para no afectar otros programas.

## Verificación de la instalación

Después de la instalación, puedes verificar que todo funciona:

```bash
# Verificar que el comando está disponible
which nx

# Verificar la versión
nx help

# Probar la interfaz gráfica
nx tui
```

## Solución de problemas

### Error de permisos
Si obtienes errores de permisos, asegúrate de:
- No ejecutar los scripts como root
- Tener permisos sudo configurados

### Error de dependencias
Si hay problemas con las dependencias:
1. Actualiza tu sistema: `sudo pacman -Syu` (Arch) o `sudo apt update && sudo apt upgrade` (Debian/Ubuntu)
2. Instala las dependencias manualmente según tu distribución
3. Ejecuta `make clean && make` para recompilar

### Error de compilación
Si hay errores de compilación:
1. Verifica que tienes todas las dependencias instaladas
2. Asegúrate de tener un compilador C actualizado
3. Revisa los logs de error para más detalles

## Comandos disponibles

Una vez instalado, NLX proporciona los siguientes comandos:

- `nx help` - Mostrar ayuda
- `nx status` - Estado del sistema
- `nx bandwidth` - Métricas de ancho de banda
- `nx connections` - Conexiones activas
- `nx interfaces` - Interfaces de red
- `nx latency` - Pruebas de latencia
- `nx tui` - Interfaz gráfica interactiva

## Archivos de instalación

- **Ejecutable:** `/usr/local/bin/nx`
- **Enlace simbólico:** `/usr/bin/nx`
- **Configuración:** `/etc/nlx/nlx.conf`
- **Documentación:** `/usr/local/share/nlx/metricas.md`
