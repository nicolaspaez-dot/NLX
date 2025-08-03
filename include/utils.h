#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <time.h>

// Constantes para límites de tamaño
#define MAX_INTERFACE_NAME 32
#define MAX_IP_ADDRESS 16
#define MAX_PROCESS_NAME 64
#define MAX_SERVER_NAME 64
#define MAX_ALERT_MESSAGE 256
#define MAX_CONNECTIONS 1000
#define MAX_LATENCY_SERVERS 10

// Estructura para estadísticas de red
typedef struct {
    uint64_t rx_bytes;      // bytes recibidos (descarga)
    uint64_t tx_bytes;      // bytes enviados (subida)
    uint64_t rx_packets;    // paquetes recibidos
    uint64_t tx_packets;    // paquetes enviados
    double rx_speed;        // velocidad de descarga (MB/s)
    double tx_speed;        // velocidad de subida (MB/s)
    double total_speed;     // velocidad total
    time_t timestamp;       // timestamp de la medición
} NetworkStats;

// Estructura para conexiones de red
typedef struct {
    char local_ip[MAX_IP_ADDRESS];
    int local_port;
    char remote_ip[MAX_IP_ADDRESS];
    int remote_port;
    char state[16];
    char process[MAX_PROCESS_NAME];
    int pid;
    time_t timestamp;
} Connection;

// Estructura para pruebas de latencia
typedef struct {
    char server[MAX_SERVER_NAME];
    double latency;         // en milisegundos
    int status;             // 0 = OK, 1 = timeout, 2 = error
    time_t timestamp;
} LatencyTest;

// Estructura para alertas
typedef struct {
    char type[32];          // "bandwidth", "connection", "latency"
    char message[MAX_ALERT_MESSAGE];
    char severity[16];      // "low", "medium", "high", "critical"
    time_t timestamp;
} Alert;

// Funciones de utilidad básicas
double bytes_to_mbps(uint64_t bytes, double time_diff);
char* format_speed(double speed);
char* format_bytes(uint64_t bytes);
char* get_process_name(int pid);
char* get_interface_name(void);
time_t get_current_timestamp(void);
void sleep_ms(int milliseconds);

#endif // UTILS_H
