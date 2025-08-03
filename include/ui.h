#ifndef UI_H
#define UI_H

#include "utils.h"

// Constantes para la interfaz
#define MAX_WIDTH 80
#define MAX_HEIGHT 24
#define REFRESH_RATE 1000  // milisegundos

// Colores disponibles
#define COLOR_NORMAL 1
#define COLOR_WARNING 2
#define COLOR_ERROR 3
#define COLOR_SUCCESS 4
#define COLOR_INFO 5

// Funciones de inicialización y limpieza
void init_ui(void);
void cleanup_ui(void);
void setup_colors(void);

// Funciones de dibujo
void draw_header(void);
void draw_footer(void);
void draw_bandwidth_section(void);
void draw_connections_section(void);
void draw_interfaces_section(void);
void draw_stats_section(void);

// Funciones de actualización
void update_bandwidth_data(void);
void update_connections_data(void);
void update_interfaces_data(void);

// Función principal de la interfaz
void run_tui(void);

// Funciones de utilidad para UI
void draw_box(int y, int x, int height, int width, const char* title);
void draw_progress_bar(int y, int x, int width, double percentage, const char* label);
void draw_table_header(int y, int x, const char* headers[], int num_headers);
void draw_table_row(int y, int x, const char* values[], int num_values);

#endif // UI_H 