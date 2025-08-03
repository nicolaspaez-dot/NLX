#!/bin/bash

echo "=== NLX - Network Linux Extension - Desinstalador ==="
echo ""

# Verificar si estamos como root
if [ "$EUID" -eq 0 ]; then
    echo "ERROR: No ejecutes este script como root. Usa: ./uninstall.sh"
    exit 1
fi

echo "ADVERTENCIA: ¿Estás seguro de que quieres desinstalar NLX?"
echo "   Esto eliminará todos los archivos de NLX del sistema."
echo ""
read -p "   Escribe 'SI' para confirmar: " confirm

if [ "$confirm" != "SI" ]; then
    echo "Desinstalación cancelada"
    exit 0
fi

echo ""
echo "Desinstalando NLX..."

# Eliminar ejecutable
if [ -f /usr/local/bin/nx ]; then
    echo "Eliminando ejecutable..."
    sudo rm -f /usr/local/bin/nx
fi

# Eliminar enlace simbólico
if [ -L /usr/bin/nx ]; then
    echo "Eliminando enlace simbólico..."
    sudo rm -f /usr/bin/nx
fi

# Eliminar archivos de configuración
if [ -d /etc/nlx ]; then
    echo "Eliminando archivos de configuración..."
    sudo rm -rf /etc/nlx
fi

# Eliminar documentación
if [ -d /usr/local/share/nlx ]; then
    echo "Eliminando documentación..."
    sudo rm -rf /usr/local/share/nlx
fi

# Verificar si los directorios están vacíos y eliminarlos
if [ -d /usr/local/share ] && [ -z "$(ls -A /usr/local/share)" ]; then
    echo "Eliminando directorio vacío /usr/local/share..."
    sudo rmdir /usr/local/share
fi

echo ""
echo "NLX desinstalado exitosamente"
echo ""
echo "NOTA: Las dependencias del sistema (ncurses, libpcap, etc.)"
echo "   no se eliminaron para no afectar otros programas."
echo "   Si quieres eliminarlas manualmente, usa tu gestor de paquetes."
echo ""
echo "¡Gracias por usar NLX!" 