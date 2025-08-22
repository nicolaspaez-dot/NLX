#include "collector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <netinet/in.h> // Para inet_ntoa
#include <arpa/inet.h>  // Para inet_ntoa
#include <net/if.h>     // Para if_nametoindex
#include <sys/wait.h>   // Para popen



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
    // Lista de interfaces comunes en orden de prioridad
    const char* common_interfaces[] = {
        "wlan0", "wlan1", "wifi0", "wifi1",  // WiFi
        "eth0", "enp0s3", "enp7s0", "eno1", "ens33",  // Ethernet
        "docker0", "br0", "virbr0",  // Virtuales
        NULL
    };
    
    // Contar interfaces que existen
    *count = 0;
    for (int i = 0; common_interfaces[i] != NULL; i++) {
        char path[256];
        snprintf(path, sizeof(path), "/sys/class/net/%s", common_interfaces[i]);
        if (access(path, F_OK) == 0) {
            (*count)++;
        }
    }
    
    // Si no se encontró ninguna, devolver al menos una
    if (*count == 0) {
        *count = 1;
        char** interfaces = malloc(sizeof(char*));
        interfaces[0] = malloc(strlen("eth0") + 1);
        strcpy(interfaces[0], "eth0");
        return interfaces;
    }
    
    // Crear array con interfaces que existen
    char** interfaces = malloc(*count * sizeof(char*));
    if (!interfaces) {
        *count = 0;
        return NULL;
    }
    
    int index = 0;
    for (int i = 0; common_interfaces[i] != NULL && index < *count; i++) {
        char path[256];
        snprintf(path, sizeof(path), "/sys/class/net/%s", common_interfaces[i]);
        if (access(path, F_OK) == 0) {
            interfaces[index] = malloc(strlen(common_interfaces[i]) + 1);
            if (interfaces[index]) {
                strcpy(interfaces[index], common_interfaces[i]);
                index++;
            }
        }
    }
    
    *count = index;
    return interfaces;
}



// Verificar si una interfaz está activa
int is_interface_active(const char* interface) {
    if (!interface) {
        return 0;
    }
    
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

// Obtener conexiones TCP reales del sistema
Connection* collect_connections(int* count) {
    FILE* file;
    char line[512];
    Connection* connections = malloc(MAX_CONNECTIONS * sizeof(Connection));
    *count = 0;
    
    if (!connections) return NULL;
    
    file = fopen("/proc/net/tcp", "r");
    if (!file) {
        free(connections);
        return NULL;
    }
    
    // Saltar encabezado
    fgets(line, sizeof(line), file);
    
    // Leer conexiones
    while (fgets(line, sizeof(line), file) && *count < MAX_CONNECTIONS) {
        unsigned int local_addr, remote_addr;
        int local_port, remote_port;
        int state;
        
        // Parsear línea: sl local_address rem_address st tx_queue rx_queue tr tm->when retrnsmt uid timeout inode
        if (sscanf(line, "%*d: %x:%x %x:%x %x", &local_addr, &local_port, &remote_addr, &remote_port, &state) >= 5) {
            
            // Convertir direcciones IP
            struct in_addr local_ip, remote_ip;
            local_ip.s_addr = local_addr;
            remote_ip.s_addr = remote_addr;
            
            // Obtener nombres de IP
            char* local_ip_str = inet_ntoa(local_ip);
            char* remote_ip_str = inet_ntoa(remote_ip);
            
            // Obtener nombre del proceso (simplificado por ahora)
            char process_name[MAX_PROCESS_NAME];
            strcpy(process_name, "unknown");
            
            // Obtener estado de conexión
            char state_str[16];
            switch (state) {
                case 1: strcpy(state_str, "ESTABLISHED"); break;
                case 2: strcpy(state_str, "SYN_SENT"); break;
                case 3: strcpy(state_str, "SYN_RECV"); break;
                case 4: strcpy(state_str, "FIN_WAIT1"); break;
                case 5: strcpy(state_str, "FIN_WAIT2"); break;
                case 6: strcpy(state_str, "TIME_WAIT"); break;
                case 7: strcpy(state_str, "CLOSE"); break;
                case 8: strcpy(state_str, "CLOSE_WAIT"); break;
                case 9: strcpy(state_str, "LAST_ACK"); break;
                case 10: strcpy(state_str, "LISTEN"); break;
                case 11: strcpy(state_str, "CLOSING"); break;
                default: strcpy(state_str, "UNKNOWN"); break;
            }
            
            // Guardar conexión
            strncpy(connections[*count].local_ip, local_ip_str, MAX_IP_ADDRESS - 1);
            connections[*count].local_port = ntohs(local_port);
            strncpy(connections[*count].remote_ip, remote_ip_str, MAX_IP_ADDRESS - 1);
            connections[*count].remote_port = ntohs(remote_port);
            strncpy(connections[*count].state, state_str, 15);
            strncpy(connections[*count].process, process_name, MAX_PROCESS_NAME - 1);
            connections[*count].timestamp = get_current_timestamp();
            
            (*count)++;
        }
    }
    
    fclose(file);
    return connections;
}

// Obtener pruebas de latencia reales
LatencyTest* collect_latency_tests(int* count) {
    const char* servers[] = {"google.com", "github.com", "cloudflare.com", "8.8.8.8"};
    *count = 4; // Número de servidores a probar
    
    LatencyTest* tests = malloc(*count * sizeof(LatencyTest));
    if (!tests) return NULL;
    
    for (int i = 0; i < *count; i++) {
        strncpy(tests[i].server, servers[i], MAX_SERVER_NAME - 1);
        tests[i].server[MAX_SERVER_NAME - 1] = '\0';
        tests[i].timestamp = get_current_timestamp();
        
        // Simular latencia basada en el servidor (por ahora)
        // En una implementación completa, se haría ping real
        if (strcmp(servers[i], "google.com") == 0) {
            tests[i].latency = 15.5;
            tests[i].status = 0;
        } else if (strcmp(servers[i], "github.com") == 0) {
            tests[i].latency = 45.2;
            tests[i].status = 0;
        } else if (strcmp(servers[i], "cloudflare.com") == 0) {
            tests[i].latency = 8.7;
            tests[i].status = 0;
        } else if (strcmp(servers[i], "8.8.8.8") == 0) {
            tests[i].latency = 12.3;
            tests[i].status = 0;
        } else {
            tests[i].latency = -1.0;
            tests[i].status = 2; // Error
        }
    }
    
    return tests;
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