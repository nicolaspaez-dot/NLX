#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

// Convertir bytes a MB/s
double bytes_to_mbps(uint64_t bytes, double time_diff) {
    if (time_diff <= 0) return 0.0;
    
    // Convertir bytes a MB y dividir por segundos
    double mb = (double)bytes / (1024.0 * 1024.0);
    return mb / time_diff;
}

// Formatear velocidad para mostrar
char* format_speed(double speed) {
    static char buffer[32];
    
    if (speed < 1.0) {
        // Mostrar en KB/s
        snprintf(buffer, sizeof(buffer), "%.2f KB/s", speed * 1024.0);
    } else if (speed < 1024.0) {
        // Mostrar en MB/s
        snprintf(buffer, sizeof(buffer), "%.2f MB/s", speed);
    } else {
        // Mostrar en GB/s
        snprintf(buffer, sizeof(buffer), "%.2f GB/s", speed / 1024.0);
    }
    
    return buffer;
}

// Formatear bytes para mostrar
char* format_bytes(uint64_t bytes) {
    static char buffer[32];
    
    if (bytes < 1024) {
        snprintf(buffer, sizeof(buffer), "%lu B", bytes);
    } else if (bytes < 1024 * 1024) {
        snprintf(buffer, sizeof(buffer), "%.1f KB", (double)bytes / 1024.0);
    } else if (bytes < 1024 * 1024 * 1024) {
        snprintf(buffer, sizeof(buffer), "%.1f MB", (double)bytes / (1024.0 * 1024.0));
    } else {
        snprintf(buffer, sizeof(buffer), "%.1f GB", (double)bytes / (1024.0 * 1024.0 * 1024.0));
    }
    
    return buffer;
}

// Obtener nombre del proceso por PID
char* get_process_name(int pid) {
    static char process_name[MAX_PROCESS_NAME];
    char path[256];
    FILE* file;
    
    // Construir ruta al archivo comm del proceso
    snprintf(path, sizeof(path), "/proc/%d/comm", pid);
    
    file = fopen(path, "r");
    if (!file) {
        snprintf(process_name, sizeof(process_name), "unknown");
        return process_name;
    }
    
    // Leer nombre del proceso
    if (fgets(process_name, sizeof(process_name), file)) {
        // Eliminar salto de línea
        process_name[strcspn(process_name, "\n")] = '\0';
    } else {
        snprintf(process_name, sizeof(process_name), "unknown");
    }
    
    fclose(file);
    return process_name;
}

// Obtener nombre de la interfaz de red principal
char* get_interface_name(void) {
    static char interface_name[MAX_INTERFACE_NAME];
    
    
    strncpy(interface_name, "eth0", sizeof(interface_name) - 1);
    interface_name[sizeof(interface_name) - 1] = '\0';
    
    return interface_name;
}

// Obtener timestamp actual
time_t get_current_timestamp(void) {
    return time(NULL);
}

// Dormir por milisegundos
void sleep_ms(int milliseconds) {
   
    if (milliseconds >= 1000) {
        sleep(milliseconds / 1000);
    }
} 