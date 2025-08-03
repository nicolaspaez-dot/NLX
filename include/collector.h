#ifndef COLLECTOR_H
#define COLLECTOR_H

#include "utils.h"

// Funciones de recolección de métricas de red
NetworkStats collect_network_stats(const char* interface);
Connection* collect_connections(int* count);
LatencyTest* collect_latency_tests(int* count);

// Funciones específicas de recolección
NetworkStats read_interface_stats(const char* interface);
int get_connection_count(void);
int get_active_processes(void);

// Funciones de cálculo de velocidades
void calculate_speeds(NetworkStats* current, NetworkStats* previous, double time_diff);
double calculate_bandwidth_usage(const char* interface);

// Funciones de detección de interfaces
char** get_available_interfaces(int* count);
int is_interface_active(const char* interface);

// Funciones de monitoreo de procesos
int get_process_network_usage(int pid, uint64_t* rx_bytes, uint64_t* tx_bytes);
char* get_process_connections(int pid, int* count);

#endif // COLLECTOR_H 