#include "renderer.h"
#include "ui.h"
#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Definir constantes de color como respaldo si no están en ui.h
#ifndef COLOR_NORMAL
#define COLOR_NORMAL 1
#define COLOR_WARNING 2
#define COLOR_ERROR 3
#define COLOR_SUCCESS 4
#define COLOR_INFO 5
#endif

// ============================================================================
// FUNCIONES DE UTILIDAD PARA RENDERIZADO
// ============================================================================

void draw_horizontal_line(int y, int x, int width, char character) {
    for (int i = 0; i < width; i++) {
        mvaddch(y, x + i, character);
    }
}

void draw_vertical_line(int y, int x, int height, char character) {
    for (int i = 0; i < height; i++) {
        mvaddch(y + i, x, character);
    }
}

void center_text(int y, int x, int width, const char* text) {
    int text_len = strlen(text);
    int start_x = x + (width - text_len) / 2;
    if (start_x < x) start_x = x;
    mvprintw(y, start_x, "%s", text);
}

void draw_box_advanced(int y, int x, int height, int width, const char* title, int style) {
    // Dibujar esquinas y bordes
    mvaddch(y, x, '+');
    mvaddch(y, x + width - 1, '+');
    mvaddch(y + height - 1, x, '+');
    mvaddch(y + height - 1, x + width - 1, '+');
    
    // Dibujar líneas horizontales
    draw_horizontal_line(y, x + 1, width - 2, '-');
    draw_horizontal_line(y + height - 1, x + 1, width - 2, '-');
    
    // Dibujar líneas verticales
    draw_vertical_line(y + 1, x, height - 2, '|');
    draw_vertical_line(y + 1, x + width - 1, height - 2, '|');
    
    // Dibujar título si existe
    if (title && strlen(title) > 0) {
        mvaddch(y, x + 2, ' ');
        mvprintw(y, x + 3, "%s", title);
        mvaddch(y, x + 3 + strlen(title), ' ');
    }
}

// ============================================================================
// GRÁFICOS DE ANCHO DE BANDA
// ============================================================================

void init_bandwidth_graph(GraphData* graph, const char* title) {
    memset(graph, 0, sizeof(GraphData));
    graph->max_values = MAX_GRAPH_WIDTH;
    graph->max_value = 1.0; // Se actualizará dinámicamente
    if (title) {
        strncpy(graph->title, title, sizeof(graph->title) - 1);
    }
}

void add_bandwidth_data(GraphData* graph, double value) {
    if (value > graph->max_value) {
        graph->max_value = value;
    }
    
    graph->values[graph->current_index] = value;
    graph->current_index = (graph->current_index + 1) % graph->max_values;
}

void draw_bandwidth_graph(int y, int x, GraphData* graph) {
    int width = MAX_GRAPH_WIDTH;
    int height = MAX_GRAPH_HEIGHT;
    
    // Dibujar caja del gráfico
    draw_box_advanced(y, x, height + 2, width + 2, graph->title, 0);
    
    // Dibujar gráfico
    for (int i = 0; i < width; i++) {
        int data_index = (graph->current_index - width + i + graph->max_values) % graph->max_values;
        double value = graph->values[data_index];
        
        if (graph->max_value > 0) {
            int bar_height = (int)((value / graph->max_value) * height);
            if (bar_height > height) bar_height = height;
            
            // Dibujar barra
            for (int j = 0; j < bar_height; j++) {
                char symbol = '|';
                if (j == bar_height - 1) symbol = '#';
                mvaddch(y + height - j, x + 1 + i, symbol);
            }
        }
    }
    
    // Mostrar valores actuales
    char info[64];
    snprintf(info, sizeof(info), "Max: %.1f MB/s", graph->max_value);
    mvprintw(y + height + 1, x + 1, "%s", info);
}

void draw_speed_graph(int y, int x, double rx_speed, double tx_speed, double max_speed) {
    int width = 40;
    int height = 8;
    
    // Dibujar caja
    draw_box_advanced(y, x, height + 2, width + 2, "Velocidades", 0);
    
    // Calcular alturas de las barras
    int rx_height = max_speed > 0 ? (int)((rx_speed / max_speed) * height) : 0;
    int tx_height = max_speed > 0 ? (int)((tx_speed / max_speed) * height) : 0;
    
    if (rx_height > height) rx_height = height;
    if (tx_height > height) tx_height = height;
    
    // Dibujar barra de descarga (azul)
    attron(COLOR_PAIR(COLOR_INFO));
    for (int i = 0; i < rx_height; i++) {
        mvaddch(y + height - i, x + 5, '|');
    }
    attroff(COLOR_PAIR(COLOR_INFO));
    
    // Dibujar barra de subida (verde)
    attron(COLOR_PAIR(COLOR_SUCCESS));
    for (int i = 0; i < tx_height; i++) {
        mvaddch(y + height - i, x + 15, '|');
    }
    attroff(COLOR_PAIR(COLOR_SUCCESS));
    
    // Etiquetas
    mvprintw(y + height + 1, x + 2, "RX: %.1f", rx_speed);
    mvprintw(y + height + 1, x + 12, "TX: %.1f", tx_speed);
}

// ============================================================================
// TABLAS
// ============================================================================

void init_connections_table(TableData* table) {
    memset(table, 0, sizeof(TableData));
    strcpy(table->title, "Conexiones Activas");
    
    // Headers por defecto
    strcpy(table->headers[0], "Puerto");
    strcpy(table->headers[1], "Estado");
    strcpy(table->headers[2], "Proceso");
    strcpy(table->headers[3], "IP");
    strcpy(table->headers[4], "Bytes");
    table->num_headers = 5;
}

void add_connection_row(TableData* table, const char* port, const char* state, 
                       const char* process, const char* ip, const char* bytes) {
    if (table->num_rows >= MAX_TABLE_ROWS) return;
    
    strncpy(table->data[table->num_rows][0], port, 31);
    strncpy(table->data[table->num_rows][1], state, 31);
    strncpy(table->data[table->num_rows][2], process, 31);
    strncpy(table->data[table->num_rows][3], ip, 31);
    strncpy(table->data[table->num_rows][4], bytes, 31);
    
    table->num_rows++;
}

void draw_connections_table(int y, int x, TableData* table) {
    int width = 80;
    int height = table->num_rows + 3;
    
    // Dibujar caja
    draw_box_advanced(y, x, height, width, table->title, 0);
    
    // Dibujar headers
    int col_width = width / table->num_headers;
    for (int i = 0; i < table->num_headers; i++) {
        int col_x = x + 1 + (i * col_width);
        attron(A_BOLD);
        mvprintw(y + 1, col_x, "%-*s", col_width - 1, table->headers[i]);
        attroff(A_BOLD);
    }
    
    // Dibujar línea separadora
    draw_horizontal_line(y + 2, x + 1, width - 2, '-');
    
    // Dibujar datos
    for (int row = 0; row < table->num_rows && row < MAX_TABLE_ROWS; row++) {
        for (int col = 0; col < table->num_headers; col++) {
            int col_x = x + 1 + (col * col_width);
            mvprintw(y + 3 + row, col_x, "%-*s", col_width - 1, table->data[row][col]);
        }
    }
}

void draw_processes_table(int y, int x, TableData* table) {
    // Similar a draw_connections_table pero para procesos
    draw_connections_table(y, x, table);
}

// ============================================================================
// BARRAS DE PROGRESO
// ============================================================================

void draw_progress_bar_advanced(int y, int x, int width, double percentage, 
                               const char* label, const char* value) {
    // Dibujar etiqueta
    mvprintw(y, x, "%s:", label);
    
    // Dibujar barra
    int bar_width = width - strlen(label) - 10;
    int filled_width = (int)((percentage / 100.0) * bar_width);
    
    mvaddch(y, x + strlen(label) + 1, '[');
    for (int i = 0; i < bar_width; i++) {
        if (i < filled_width) {
            mvaddch(y, x + strlen(label) + 2 + i, '#');
        } else {
            mvaddch(y, x + strlen(label) + 2 + i, ' ');
        }
    }
    mvaddch(y, x + strlen(label) + 2 + bar_width, ']');
    
    // Dibujar valor
    mvprintw(y, x + strlen(label) + 3 + bar_width, " %s", value);
}

void draw_latency_bars(int y, int x, const char* servers[], double latencies[], int count) {
    draw_box_advanced(y, x, count + 2, 50, "Latencia", 0);
    
    for (int i = 0; i < count; i++) {
        int bar_y = y + 1 + i;
        int bar_width = 30;
        
        // Nombre del servidor
        mvprintw(bar_y, x + 1, "%-15s", servers[i]);
        
        // Barra de latencia
        double max_latency = 100.0; // 100ms máximo
        int filled_width = latencies[i] > max_latency ? bar_width : 
                          (int)((latencies[i] / max_latency) * bar_width);
        
        // Color según latencia
        if (latencies[i] < 20) {
            attron(COLOR_PAIR(COLOR_SUCCESS));
        } else if (latencies[i] < 50) {
            attron(COLOR_PAIR(COLOR_INFO));
        } else {
            attron(COLOR_PAIR(COLOR_WARNING));
        }
        
        for (int j = 0; j < bar_width; j++) {
            if (j < filled_width) {
                mvaddch(bar_y, x + 16 + j, '#');
            } else {
                mvaddch(bar_y, x + 16 + j, ' ');
            }
        }
        attroff(COLOR_PAIR(COLOR_SUCCESS) | COLOR_PAIR(COLOR_INFO) | COLOR_PAIR(COLOR_WARNING));
        
        // Valor de latencia
        mvprintw(bar_y, x + 47, "%.1fms", latencies[i]);
    }
}

// ============================================================================
// HISTOGRAMAS
// ============================================================================

void draw_histogram(int y, int x, int height, int width, double values[], int count, const char* title) {
    draw_box_advanced(y, x, height + 2, width + 2, title, 0);
    
    if (count == 0) return;
    
    // Encontrar valor máximo
    double max_value = values[0];
    for (int i = 1; i < count; i++) {
        if (values[i] > max_value) max_value = values[i];
    }
    
    if (max_value <= 0) return;
    
    // Dibujar histograma
    int bar_width = width / count;
    for (int i = 0; i < count && i < width; i++) {
        int bar_height = (int)((values[i] / max_value) * height);
        if (bar_height > height) bar_height = height;
        
        for (int j = 0; j < bar_height; j++) {
            mvaddch(y + height - j, x + 1 + (i * bar_width), '#');
        }
    }
} 