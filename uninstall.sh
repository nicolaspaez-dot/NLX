#!/bin/bash

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== NLX - Network Linux Extension - Desinstalador ===${NC}"
echo ""

# Verificar si estamos como root
if [ "$EUID" -eq 0 ]; then
    echo -e "${RED}ERROR: No ejecutes este script como root. Usa: ./uninstall.sh${NC}"
    exit 1
fi

# Verificar si NLX está instalado
if [ ! -f /usr/local/bin/nx ] && [ ! -L /usr/bin/nx ]; then
    echo -e "${YELLOW}NLX no parece estar instalado en el sistema.${NC}"
    echo -e "${YELLOW}No hay nada que desinstalar.${NC}"
    exit 0
fi

echo -e "${YELLOW}ADVERTENCIA: ¿Estás seguro de que quieres desinstalar NLX?${NC}"
echo -e "${YELLOW}   Esto eliminará todos los archivos de NLX del sistema.${NC}"
echo ""
read -p "   Escribe 'SI' para confirmar: " confirm

if [ "$confirm" != "SI" ]; then
    echo -e "${BLUE}Desinstalación cancelada${NC}"
    exit 0
fi

echo ""
echo -e "${BLUE}Desinstalando NLX...${NC}"

# Función para eliminar archivo con verificación
remove_file() {
    local file="$1"
    local description="$2"
    
    if [ -f "$file" ] || [ -L "$file" ]; then
        echo -e "${BLUE}Eliminando $description...${NC}"
        sudo rm -f "$file"
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✓ $description eliminado${NC}"
        else
            echo -e "${RED}✗ Error al eliminar $description${NC}"
        fi
    else
        echo -e "${YELLOW}- $description no encontrado${NC}"
    fi
}

# Función para eliminar directorio con verificación
remove_directory() {
    local dir="$1"
    local description="$2"
    
    if [ -d "$dir" ]; then
        echo -e "${BLUE}Eliminando $description...${NC}"
        sudo rm -rf "$dir"
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✓ $description eliminado${NC}"
        else
            echo -e "${RED}✗ Error al eliminar $description${NC}"
        fi
    else
        echo -e "${YELLOW}- $description no encontrado${NC}"
    fi
}

# Eliminar ejecutable
remove_file "/usr/local/bin/nx" "ejecutable principal"

# Eliminar enlace simbólico
remove_file "/usr/bin/nx" "enlace simbólico"

# Eliminar archivos de configuración
remove_directory "/etc/nlx" "archivos de configuración"

# Eliminar documentación
remove_directory "/usr/local/share/nlx" "documentación"

# Verificar si los directorios están vacíos y eliminarlos
if [ -d /usr/local/share ] && [ -z "$(ls -A /usr/local/share 2>/dev/null)" ]; then
    echo -e "${BLUE}Eliminando directorio vacío /usr/local/share...${NC}"
    sudo rmdir /usr/local/share
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Directorio /usr/local/share eliminado${NC}"
    fi
fi

if [ -d /usr/local/bin ] && [ -z "$(ls -A /usr/local/bin 2>/dev/null)" ]; then
    echo -e "${BLUE}Eliminando directorio vacío /usr/local/bin...${NC}"
    sudo rmdir /usr/local/bin
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Directorio /usr/local/bin eliminado${NC}"
    fi
fi

echo ""
echo -e "${GREEN}NLX desinstalado exitosamente${NC}"
echo ""
echo -e "${YELLOW}NOTA: Las dependencias del sistema (ncurses, libpcap, etc.)${NC}"
echo -e "${YELLOW}   no se eliminaron para no afectar otros programas.${NC}"
echo -e "${YELLOW}   Si quieres eliminarlas manualmente, usa tu gestor de paquetes.${NC}"
echo ""
echo -e "${BLUE}Dependencias que puedes eliminar manualmente:${NC}"
echo "  - build-essential (o gcc, make)"
echo "  - libncurses5-dev (o ncurses-devel)"
echo "  - libpcap-dev (o libpcap-devel)"
echo "  - libcurl4-openssl-dev (o libcurl-devel)"
echo "  - iproute2 (o iproute)"
echo "  - iputils-ping (o iputils)"
echo ""
echo -e "${GREEN}¡Gracias por usar NLX!${NC}" 