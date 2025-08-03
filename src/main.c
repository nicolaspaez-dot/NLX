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

// Función para mostrar conexiones reales
void show_connections(void) {
    printf("NLX - Conexiones Activas\n");
    printf("=========================\n\n");
    
    int count;
    Connection* connections = collect_connections(&count);
    
    if (!connections || count == 0) {
        printf("No se pudieron obtener las conexiones\n");
        return;
    }
    
    printf("Conexiones TCP activas: %d\n\n", count);
    
    // Mostrar las primeras 10 conexiones más relevantes
    int to_show = count > 10 ? 10 : count;
    printf("Primeras %d conexiones:\n", to_show);
    printf("%-15s %-8s %-15s %-8s %-12s\n", "IP Local", "Puerto", "IP Remota", "Puerto", "Estado");
    printf("------------------------------------------------------------\n");
    
    for (int i = 0; i < to_show; i++) {
        printf("%-15s %-8d %-15s %-8d %-12s\n",
               connections[i].local_ip,
               connections[i].local_port,
               connections[i].remote_ip,
               connections[i].remote_port,
               connections[i].state);
    }
    
    if (count > 10) {
        printf("\n... y %d conexiones más\n", count - 10);
    }
    
    free(connections);
}

// Función para mostrar latencia real
void show_latency(void) {
    printf("NLX - Pruebas de Latencia\n");
    printf("========================\n\n");
    
    int count;
    LatencyTest* tests = collect_latency_tests(&count);
    
    if (!tests) {
        printf("No se pudieron realizar las pruebas de latencia\n");
        return;
    }
    
    printf("Probando conectividad a %d servidores...\n\n", count);
    printf("%-15s %-12s %-10s\n", "Servidor", "Latencia", "Estado");
    printf("----------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%-15s ", tests[i].server);
        
        if (tests[i].status == 0 && tests[i].latency > 0) {
            printf("%-12.1fms ", tests[i].latency);
            
            // Color según latencia
            if (tests[i].latency < 20) {
                printf("✅ Excelente");
            } else if (tests[i].latency < 50) {
                printf("🟡 Buena");
            } else if (tests[i].latency < 100) {
                printf("🟠 Regular");
            } else {
                printf("🔴 Lenta");
            }
        } else if (tests[i].status == 1) {
            printf("%-12s ⏰ Timeout", "N/A");
        } else {
            printf("%-12s ❌ Error", "N/A");
        }
        printf("\n");
    }
    
    printf("\nResumen:\n");
    int good_connections = 0;
    double total_latency = 0.0;
    int valid_tests = 0;
    
    for (int i = 0; i < count; i++) {
        if (tests[i].status == 0 && tests[i].latency > 0) {
            good_connections++;
            total_latency += tests[i].latency;
            valid_tests++;
        }
    }
    
    printf("  Conexiones exitosas: %d/%d\n", good_connections, count);
    if (valid_tests > 0) {
        printf("  Latencia promedio: %.1fms\n", total_latency / valid_tests);
    }
    
    free(tests);
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