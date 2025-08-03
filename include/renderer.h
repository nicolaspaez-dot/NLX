#ifndef RENDERER_H
#define RENDERER_H

#include "utils.h"

// Constantes para gráficos
#define MAX_GRAPH_WIDTH 200
#define MAX_GRAPH_HEIGHT 10
#define MAX_TABLE_ROWS 20
#define MAX_TABLE_COLS 5

// Estructura para datos de gráfico
typedef struct {
    double values[MAX_GRAPH_WIDTH];
    int current_index;
    int max_values;
    double max_value;
    char title[64];
} GraphData;

// Estructura para datos de tabla
typedef struct {
    char headers[MAX_TABLE_COLS][32];
    char data[MAX_TABLE_ROWS][MAX_TABLE_COLS][32];
    int num_headers;
    int num_rows;
    char title[64];
} TableData;

// Funciones de gráficos de ancho de banda
void init_bandwidth_graph(GraphData* graph, const char* title);
void add_bandwidth_data(GraphData* graph, double value);
void draw_bandwidth_graph(int y, int x, GraphData* graph);
void draw_speed_graph(int y, int x, double rx_speed, double tx_speed, double max_speed);

// Funciones de tablas
void init_connections_table(TableData* table);
void add_connection_row(TableData* table, const char* port, const char* state, 
                       const char* process, const char* ip, const char* bytes);
void draw_connections_table(int y, int x, TableData* table);
void draw_processes_table(int y, int x, TableData* table);

// Funciones de barras de progreso
void draw_progress_bar_advanced(int y, int x, int width, double percentage, 
                               const char* label, const char* value);
void draw_latency_bars(int y, int x, const char* servers[], double latencies[], int count);

// Funciones de utilidad para renderizado
void draw_horizontal_line(int y, int x, int width, char character);
void draw_vertical_line(int y, int x, int height, char character);
void draw_box_advanced(int y, int x, int height, int width, const char* title, int style);
void center_text(int y, int x, int width, const char* text);

// Funciones de histogramas
void draw_histogram(int y, int x, int height, int width, double values[], int count, const char* title);

#endif // RENDERER_H 