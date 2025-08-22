#!/bin/bash

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== NLX - Network Linux Extension - Instalador ===${NC}"
echo ""

# Verificar si estamos como root
if [ "$EUID" -eq 0 ]; then
    echo -e "${RED}ERROR: No ejecutes este script como root. Usa: ./install.sh${NC}"
    exit 1
fi

# Función para detectar distribución
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$ID
        DISTRO_VERSION=$VERSION_ID
    elif [ -f /etc/debian_version ]; then
        DISTRO="debian"
        DISTRO_VERSION=$(cat /etc/debian_version)
    elif [ -f /etc/arch-release ]; then
        DISTRO="arch"
        DISTRO_VERSION="rolling"
    elif [ -f /etc/redhat-release ]; then
        DISTRO="redhat"
        DISTRO_VERSION=$(cat /etc/redhat-release | grep -oE '[0-9]+\.[0-9]+' | head -1)
    else
        DISTRO="unknown"
        DISTRO_VERSION="unknown"
    fi
}

# Detectar distribución
detect_distro

echo -e "${BLUE}Distribución detectada: ${GREEN}$DISTRO${NC} (versión: $DISTRO_VERSION)"
echo ""

# Función para instalar dependencias en Debian/Ubuntu
install_debian_deps() {
    echo -e "${YELLOW}Instalando dependencias para Debian/Ubuntu...${NC}"
    sudo apt-get update
    sudo apt-get install -y build-essential libncurses5-dev libpcap-dev libcurl4-openssl-dev iproute2 iputils-ping
}

# Función para instalar dependencias en Arch
install_arch_deps() {
    echo -e "${YELLOW}Instalando dependencias para Arch Linux...${NC}"
    sudo pacman -Sy --noconfirm base-devel ncurses libpcap curl iproute2 iputils
}

# Función para instalar dependencias en Fedora
install_fedora_deps() {
    echo -e "${YELLOW}Instalando dependencias para Fedora...${NC}"
    sudo dnf install -y gcc make ncurses-devel libpcap-devel libcurl-devel iproute iputils
}

# Función para instalar dependencias en RHEL/CentOS
install_redhat_deps() {
    echo -e "${YELLOW}Instalando dependencias para RHEL/CentOS...${NC}"
    if command -v dnf &> /dev/null; then
        sudo dnf install -y gcc make ncurses-devel libpcap-devel libcurl-devel iproute iputils
    else
        sudo yum install -y gcc make ncurses-devel libpcap-devel libcurl-devel iproute iputils
    fi
}

# Función para instalar dependencias en openSUSE
install_opensuse_deps() {
    echo -e "${YELLOW}Instalando dependencias para openSUSE...${NC}"
    sudo zypper install -y gcc make ncurses-devel libpcap-devel libcurl-devel iproute2 iputils
}

# Instalar dependencias según la distribución
echo -e "${BLUE}Instalando dependencias...${NC}"

case $DISTRO in
    "debian"|"ubuntu"|"linuxmint"|"pop"|"elementary"|"kali"|"parrot")
        install_debian_deps
        ;;
    "arch"|"manjaro"|"endeavouros"|"garuda"|"artix")
        install_arch_deps
        ;;
    "fedora"|"nobara")
        install_fedora_deps
        ;;
    "rhel"|"centos"|"rocky"|"alma"|"oracle")
        install_redhat_deps
        ;;
    "opensuse"|"tumbleweed"|"leap")
        install_opensuse_deps
        ;;
    *)
        echo -e "${RED}ERROR: Distribución no soportada: $DISTRO${NC}"
        echo -e "${YELLOW}Distribuciones soportadas:${NC}"
        echo "  - Debian/Ubuntu y derivadas (Linux Mint, Pop!_OS, Elementary, Kali, Parrot)"
        echo "  - Arch Linux y derivadas (Manjaro, EndeavourOS, Garuda, Artix)"
        echo "  - Fedora y derivadas (Nobara)"
        echo "  - RHEL/CentOS y derivadas (Rocky Linux, AlmaLinux, Oracle Linux)"
        echo "  - openSUSE (Tumbleweed, Leap)"
        echo ""
        echo -e "${YELLOW}Instala manualmente las dependencias:${NC}"
        echo "  - build-essential (o gcc, make)"
        echo "  - libncurses5-dev (o ncurses-devel)"
        echo "  - libpcap-dev (o libpcap-devel)"
        echo "  - libcurl4-openssl-dev (o libcurl-devel)"
        echo "  - iproute2 (o iproute)"
        echo "  - iputils-ping (o iputils)"
        exit 1
        ;;
esac

if [ $? -eq 0 ]; then
    echo -e "${GREEN}Dependencias instaladas correctamente${NC}"
else
    echo -e "${RED}ERROR: Error al instalar dependencias${NC}"
    exit 1
fi

echo ""

# Compilar NLX
echo -e "${BLUE}Compilando NLX...${NC}"
if make clean && make; then
    echo -e "${GREEN}Compilación exitosa${NC}"
else
    echo -e "${RED}ERROR: Error en la compilación${NC}"
    exit 1
fi

# Crear directorios de instalación
echo -e "${BLUE}Creando directorios...${NC}"
sudo mkdir -p /usr/local/bin
sudo mkdir -p /usr/local/share/nlx
sudo mkdir -p /etc/nlx

# Instalar ejecutable
echo -e "${BLUE}Instalando ejecutable...${NC}"
sudo cp build/nx /usr/local/bin/nx
sudo chmod +x /usr/local/bin/nx

# Instalar archivos de configuración
echo -e "${BLUE}Instalando archivos de configuración...${NC}"
sudo cp config/nlx.conf /etc/nlx/nlx.conf
sudo cp metricas.md /usr/local/share/nlx/metricas.md

# Crear enlace simbólico
if [ ! -f /usr/bin/nx ]; then
    echo -e "${BLUE}Creando enlace simbólico...${NC}"
    sudo ln -sf /usr/local/bin/nx /usr/bin/nx
fi

echo ""
echo -e "${GREEN}NLX instalado exitosamente!${NC}"
echo ""
echo -e "${BLUE}Comandos disponibles:${NC}"
echo "  nx help        - Mostrar ayuda"
echo "  nx status      - Estado del sistema"
echo "  nx bandwidth   - Métricas de ancho de banda"
echo "  nx connections - Conexiones activas"
echo "  nx interfaces  - Interfaces de red"
echo "  nx latency     - Pruebas de latencia"
echo "  nx tui         - Interfaz gráfica"
echo ""
echo -e "${BLUE}Documentación:${NC} /usr/local/share/nlx/metricas.md"
echo -e "${BLUE}Configuración:${NC} /etc/nlx/nlx.conf"
echo ""
echo -e "${GREEN}¡Disfruta monitoreando tu red!${NC}" 