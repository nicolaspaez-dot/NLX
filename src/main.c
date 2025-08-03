#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"
#include "collector.h"
#include "ui.h"

// Función para mostrar ayuda
void show_help(void) {
    printf("NLX - Network Linux Extension\n");
    printf("Uso: nx [comando] [opciones]\n\n");
    printf("Comandos disponibles:\n");
    printf("  help                    - Mostrar esta ayuda\n");
    printf("  status                  - Mostrar estado actual del sistema\n");
    printf("  bandwidth               - Mostrar métricas de ancho de banda\n");
    printf("  connections             - Mostrar conexiones activas\n");
    printf("  interfaces              - Mostrar interfaces de red\n");
    printf("  latency                 - Mostrar pruebas de latencia\n");
    printf("  processes               - Mostrar procesos activos\n");
    printf("  tui                     - Interfaz gráfica en terminal\n\n");
    printf("Ejemplos:\n");
    printf("  nx help                 - Mostrar esta ayuda\n");
    printf("  nx status               - Estado del sistema\n");
    printf("  nx bandwidth            - Métricas de ancho de banda\n");
    printf("  nx tui                  - Interfaz gráfica interactiva\n");
}

// Función para mostrar estado del sistema
void show_status(void) {
    printf("NLX - Estado del Sistema\n");
    printf("========================\n\n");
    
    // Interfaces de red
    printf("Interfaces de Red:\n");
    int interface_count;
    char** interfaces = get_available_interfaces(&interface_count);
    
    if (interfaces) {
        for (int i = 0; i < interface_count; i++) {
            int is_active = is_interface_active(interfaces[i]);
            printf("  %s (%s)\n", interfaces[i], is_active ? "activa" : "inactiva");
        }
        
        // Liberar memoria
        for (int i = 0; i < interface_count; i++) {
            free(interfaces[i]);
        }
        free(interfaces);
    }
    printf("\n");
    
    // Estadísticas del sistema
    printf("Estadísticas del Sistema:\n");
    int connections = get_connection_count();
    int processes = get_active_processes();
    printf("  Conexiones TCP activas: %d\n", connections);
    printf("  Procesos activos: %d\n", processes);
    printf("\n");
    
    // Timestamp
    time_t now = get_current_timestamp();
    printf("Última actualización: %ld\n", now);
}

// Función para mostrar métricas de ancho de banda
void show_bandwidth(void) {
    printf("NLX - Métricas de Ancho de Banda\n");
    printf("================================\n\n");
    
    // Obtener interfaz activa (por ahora usamos la primera disponible)
    int interface_count;
    char** interfaces = get_available_interfaces(&interface_count);
    
    if (!interfaces || interface_count == 0) {
        printf("No se encontraron interfaces de red\n");
        return;
    }
    
    // Buscar primera interfaz activa
    char* active_interface = NULL;
    for (int i = 0; i < interface_count; i++) {
        if (is_interface_active(interfaces[i])) {
            active_interface = interfaces[i];
            break;
        }
    }
    
    if (!active_interface) {
        printf("No se encontró ninguna interfaz activa\n");
        return;
    }
    
    printf("Interfaz: %s\n\n", active_interface);
    
    // Tomar primera medición
    NetworkStats stats1 = collect_network_stats(active_interface);
    printf("Medición inicial:\n");
    printf("  Bytes recibidos: %s\n", format_bytes(stats1.rx_bytes));
    printf("  Bytes enviados: %s\n", format_bytes(stats1.tx_bytes));
    printf("  Paquetes recibidos: %lu\n", stats1.rx_packets);
    printf("  Paquetes enviados: %lu\n", stats1.tx_packets);
    printf("\n");
    
    printf("Esperando 2 segundos para calcular velocidades...\n");
    sleep(2);
    
    // Tomar segunda medición
    NetworkStats stats2 = collect_network_stats(active_interface);
    calculate_speeds(&stats2, &stats1, 2.0);
    
    printf("Velocidades (últimos 2 segundos):\n");
    printf("  Descarga: %s\n", format_speed(stats2.rx_speed));
    printf("  Subida: %s\n", format_speed(stats2.tx_speed));
    printf("  Total: %s\n", format_speed(stats2.total_speed));
    
    // Liberar memoria
    for (int i = 0; i < interface_count; i++) {
        free(interfaces[i]);
    }
    free(interfaces);
}

// Función para mostrar interfaces
void show_interfaces(void) {
    printf("NLX - Interfaces de Red\n");
    printf("=======================\n\n");
    
    int interface_count;
    char** interfaces = get_available_interfaces(&interface_count);
    
    if (!interfaces || interface_count == 0) {
        printf("No se encontraron interfaces de red\n");
        return;
    }
    
    printf("Interfaces disponibles:\n");
    for (int i = 0; i < interface_count; i++) {
        int is_active = is_interface_active(interfaces[i]);
        NetworkStats stats = collect_network_stats(interfaces[i]);
        
        printf("  %s:\n", interfaces[i]);
        printf("    Estado: %s\n", is_active ? "activa" : "inactiva");
        printf("    Bytes recibidos: %s\n", format_bytes(stats.rx_bytes));
        printf("    Bytes enviados: %s\n", format_bytes(stats.tx_bytes));
        printf("\n");
    }
    
    // Liberar memoria
    for (int i = 0; i < interface_count; i++) {
        free(interfaces[i]);
    }
    free(interfaces);
}

// Función para mostrar procesos
void show_processes(void) {
    printf("NLX - Procesos Activos\n");
    printf("======================\n\n");
    
    int processes = get_active_processes();
    printf("Total de procesos activos: %d\n", processes);
    printf("Proceso actual (PID %d): %s\n", getpid(), get_process_name(getpid()));
}

// Función para mostrar conexiones (placeholder)
void show_connections(void) {
    printf("NLX - Conexiones Activas\n");
    printf("=========================\n\n");
    
    int connections = get_connection_count();
    printf("Conexiones TCP activas: %d\n", connections);
    printf("(Funcionalidad completa en desarrollo)\n");
}

// Función para mostrar latencia (placeholder)
void show_latency(void) {
    printf("NLX - Pruebas de Latencia\n");
    printf("========================\n\n");
    
    printf("(Funcionalidad en desarrollo)\n");
    printf("Servidores a probar: google.com, github.com, cloudflare.com\n");
}

// Función para ejecutar interfaz TUI
void show_tui(void) {
    run_tui();
}

int main(int argc, char* argv[]) {
    // Si no hay argumentos, mostrar ayuda
    if (argc < 2) {
        show_help();
        return 0;
    }
    
    // Parsear comando
    if (strcmp(argv[1], "help") == 0) {
        show_help();
    }
    else if (strcmp(argv[1], "status") == 0) {
        show_status();
    }
    else if (strcmp(argv[1], "bandwidth") == 0) {
        show_bandwidth();
    }
    else if (strcmp(argv[1], "interfaces") == 0) {
        show_interfaces();
    }
    else if (strcmp(argv[1], "processes") == 0) {
        show_processes();
    }
    else if (strcmp(argv[1], "connections") == 0) {
        show_connections();
    }
    else if (strcmp(argv[1], "latency") == 0) {
        show_latency();
    }
    else if (strcmp(argv[1], "tui") == 0) {
        show_tui();
    }
    else {
        printf("Comando desconocido: %s\n", argv[1]);
        printf("Usa 'nx help' para ver comandos disponibles\n");
        return 1;
    }
    
    return 0;
} 