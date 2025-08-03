#!/bin/bash

echo "=== NLX - Network Linux Extension - Instalador ==="
echo ""

# Detectar distribución
if [ -f /etc/debian_version ]; then
    DISTRO="debian"
elif [ -f /etc/arch-release ]; then
    DISTRO="arch"
elif [ -f /etc/redhat-release ]; then
    DISTRO="redhat"
else
    DISTRO="unknown"
fi

echo "Distribución detectada: $DISTRO"
echo ""

# Instalar dependencias según la distribución
echo "Instalando dependencias..."

case $DISTRO in
    "debian")
        sudo apt-get update
        sudo apt-get install -y build-essential libncurses5-dev libpcap-dev libcurl4-openssl-dev iproute2 iputils-ping
        ;;
    "arch")
        sudo pacman -Sy --noconfirm base-devel ncurses libpcap curl iproute2 iputils
        ;;
    "redhat")
        sudo yum install -y gcc make ncurses-devel libpcap-devel libcurl-devel iproute iputils
        ;;
    *)
        echo "ERROR: Distribución no soportada. Instala manualmente las dependencias."
        exit 1
        ;;
esac

echo "Dependencias instaladas correctamente"
echo ""

# Compilar NLX
echo "Compilando NLX..."
if make clean && make; then
    echo "Compilación exitosa"
else
    echo "ERROR: Error en la compilación"
    exit 1
fi

# Crear directorios de instalación
echo "Creando directorios..."
sudo mkdir -p /usr/local/bin
sudo mkdir -p /usr/local/share/nlx
sudo mkdir -p /etc/nlx

# Instalar ejecutable
echo "Instalando ejecutable..."
sudo cp build/nx /usr/local/bin/nx
sudo chmod +x /usr/local/bin/nx

# Instalar archivos de configuración
echo "Instalando archivos de configuración..."
sudo cp config/nlx.conf /etc/nlx/nlx.conf
sudo cp metricas.md /usr/local/share/nlx/metricas.md

# Crear enlace simbólico
if [ ! -f /usr/bin/nx ]; then
    echo "Creando enlace simbólico..."
    sudo ln -sf /usr/local/bin/nx /usr/bin/nx
fi

echo ""
echo "NLX instalado exitosamente!"
echo ""
echo "Comandos disponibles:"
echo "  nx help        - Mostrar ayuda"
echo "  nx status      - Estado del sistema"
echo "  nx bandwidth   - Métricas de ancho de banda"
echo "  nx connections - Conexiones activas"
echo "  nx interfaces  - Interfaces de red"
echo "  nx latency     - Pruebas de latencia"
echo "  nx tui         - Interfaz gráfica"
echo ""
echo "Documentación: /usr/local/share/nlx/metricas.md"
echo "Configuración: /etc/nlx/nlx.conf"
echo ""
echo "¡Disfruta monitoreando tu red!" 