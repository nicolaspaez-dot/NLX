#include "collector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>

// Leer estadísticas de una interfaz de red desde /proc/net/dev
NetworkStats read_interface_stats(const char* interface) {
    NetworkStats stats = {0};
    FILE* file;
    char line[512];
    char interface_name[64];
    
    // Inicializar estructura
    stats.timestamp = get_current_timestamp();
    
    file = fopen("/proc/net/dev", "r");
    if (!file) {
        return stats;
    }
    
    // Saltar las dos primeras líneas (encabezados)
    fgets(line, sizeof(line), file);
    fgets(line, sizeof(line), file);
    
    // Buscar la interfaz específica
    while (fgets(line, sizeof(line), file)) {
        // Parsear línea: interfaz: rx_bytes rx_packets rx_errors rx_drop ... tx_bytes tx_packets ...
        if (sscanf(line, "%63s %lu %lu %*u %*u %*u %*u %*u %*u %lu %lu %*u %*u %*u %*u %*u %*u",
                   interface_name, &stats.rx_bytes, &stats.rx_packets, &stats.tx_bytes, &stats.tx_packets) >= 4) {
            
            // Eliminar ":" del nombre de la interfaz
            char* colon = strchr(interface_name, ':');
            if (colon) *colon = '\0';
            
            // Verificar si es la interfaz que buscamos
            if (strcmp(interface_name, interface) == 0) {
                break;
            }
        }
    }
    
    fclose(file);
    return stats;
}

// Calcular velocidades basadas en estadísticas actuales y previas
void calculate_speeds(NetworkStats* current, NetworkStats* previous, double time_diff) {
    if (!previous || time_diff <= 0) {
        current->rx_speed = 0.0;
        current->tx_speed = 0.0;
        current->total_speed = 0.0;
        return;
    }
    
    // Calcular bytes transferidos
    uint64_t rx_diff = current->rx_bytes - previous->rx_bytes;
    uint64_t tx_diff = current->tx_bytes - previous->tx_bytes;
    
    // Convertir a MB/s
    current->rx_speed = bytes_to_mbps(rx_diff, time_diff);
    current->tx_speed = bytes_to_mbps(tx_diff, time_diff);
    current->total_speed = current->rx_speed + current->tx_speed;
}

// Obtener estadísticas de red para una interfaz
NetworkStats collect_network_stats(const char* interface) {
    return read_interface_stats(interface);
}

// Obtener interfaces de red disponibles
char** get_available_interfaces(int* count) {
    char** interfaces = NULL;
    *count = 0;
    FILE* file;
    char line[512];
    char interface_name[64];
    
    file = fopen("/proc/net/dev", "r");
    if (!file) {
        return NULL;
    }
    
    // Saltar encabezados
    fgets(line, sizeof(line), file);
    fgets(line, sizeof(line), file);
    
    // Contar interfaces
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%63s", interface_name) == 1) {
            char* colon = strchr(interface_name, ':');
            if (colon) *colon = '\0';
            
            // Ignorar lo (loopback)
            if (strcmp(interface_name, "lo") != 0) {
                (*count)++;
            }
        }
    }
    
    // Asignar memoria y llenar array
    if (*count > 0) {
        interfaces = malloc(*count * sizeof(char*));
        rewind(file);
        
        // Saltar encabezados nuevamente
        fgets(line, sizeof(line), file);
        fgets(line, sizeof(line), file);
        
        int index = 0;
        while (fgets(line, sizeof(line), file) && index < *count) {
            if (sscanf(line, "%63s", interface_name) == 1) {
                char* colon = strchr(interface_name, ':');
                if (colon) *colon = '\0';
                
                if (strcmp(interface_name, "lo") != 0) {
                    interfaces[index] = malloc(strlen(interface_name) + 1);
                    strcpy(interfaces[index], interface_name);
                    index++;
                }
            }
        }
    }
    
    fclose(file);
    return interfaces;
}

// Verificar si una interfaz está activa
int is_interface_active(const char* interface) {
    char path[256];
    FILE* file;
    char operstate[32];
    
    snprintf(path, sizeof(path), "/sys/class/net/%s/operstate", interface);
    file = fopen(path, "r");
    if (!file) {
        return 0;
    }
    
    if (fgets(operstate, sizeof(operstate), file)) {
        operstate[strcspn(operstate, "\n")] = '\0';
        fclose(file);
        return strcmp(operstate, "up") == 0;
    }
    
    fclose(file);
    return 0;
}

// Obtener número de conexiones activas
int get_connection_count(void) {
    FILE* file;
    char line[512];
    int count = 0;
    
    file = fopen("/proc/net/tcp", "r");
    if (!file) {
        return 0;
    }
    
    // Saltar encabezado
    fgets(line, sizeof(line), file);
    
    // Contar líneas (conexiones)
    while (fgets(line, sizeof(line), file)) {
        count++;
    }
    
    fclose(file);
    return count;
}

// Obtener número de procesos activos
int get_active_processes(void) {
    DIR* dir;
    struct dirent* entry;
    int count = 0;
    
    dir = opendir("/proc");
    if (!dir) {
        return 0;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        // Verificar si es un directorio numérico (PID)
        if (entry->d_type == 4 && isdigit(entry->d_name[0])) { // 4 = DT_DIR
            count++;
        }
    }
    
    closedir(dir);
    return count;
}

// Funciones placeholder para futuras implementaciones
Connection* collect_connections(int* count) {
    *count = 0;
    // TODO: Implementar lectura de /proc/net/tcp y /proc/net/udp
    return NULL;
}

LatencyTest* collect_latency_tests(int* count) {
    *count = 0;
    // TODO: Implementar pruebas de ping
    return NULL;
}

double calculate_bandwidth_usage(const char* interface) {
    // TODO: Implementar cálculo de uso de ancho de banda
    return 0.0;
}

int get_process_network_usage(int pid, uint64_t* rx_bytes, uint64_t* tx_bytes) {
    // TODO: Implementar monitoreo de uso de red por proceso
    *rx_bytes = 0;
    *tx_bytes = 0;
    return 0;
}

char* get_process_connections(int pid, int* count) {
    // TODO: Implementar obtención de conexiones por proceso
    *count = 0;
    return NULL;
} 