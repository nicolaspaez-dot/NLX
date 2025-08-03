#include "ui.h"
#include "collector.h"
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

// Variables globales para datos
static NetworkStats current_stats = {0};
static NetworkStats previous_stats = {0};
static char* current_interface = NULL;
static int connections_count = 0;
static int processes_count = 0;

// Inicializar ncurses
void init_ui(void) {
    initscr();              // Inicializar pantalla
    noecho();               // No mostrar teclas presionadas
    curs_set(0);            // Ocultar cursor
    keypad(stdscr, TRUE);   // Habilitar teclas especiales
    cbreak();               // Modo cbreak para input inmediato
    nodelay(stdscr, TRUE);  // getch() no bloquea
    
    setup_colors();
}

// Configurar colores
void setup_colors(void) {
    if (has_colors()) {
        start_color();
        init_pair(COLOR_NORMAL, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_WARNING, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOR_ERROR, COLOR_RED, COLOR_BLACK);
        init_pair(COLOR_SUCCESS, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_INFO, COLOR_CYAN, COLOR_BLACK);
    }
}

// Limpiar ncurses
void cleanup_ui(void) {
    endwin();
}

// Dibujar caja con título
void draw_box(int y, int x, int height, int width, const char* title) {
    // Dibujar esquinas
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + width - 1, ACS_URCORNER);
    mvaddch(y + height - 1, x, ACS_LLCORNER);
    mvaddch(y + height - 1, x + width - 1, ACS_LRCORNER);
    
    // Dibujar líneas horizontales
    for (int i = 1; i < width - 1; i++) {
        mvaddch(y, x + i, ACS_HLINE);
        mvaddch(y + height - 1, x + i, ACS_HLINE);
    }
    
    // Dibujar líneas verticales
    for (int i = 1; i < height - 1; i++) {
        mvaddch(y + i, x, ACS_VLINE);
        mvaddch(y + i, x + width - 1, ACS_VLINE);
    }
    
    // Dibujar título si existe
    if (title && strlen(title) > 0) {
        attron(COLOR_PAIR(COLOR_INFO));
        mvprintw(y, x + 2, " %s ", title);
        attroff(COLOR_PAIR(COLOR_INFO));
    }
}

// Dibujar barra de progreso
void draw_progress_bar(int y, int x, int width, double percentage, const char* label) {
    if (percentage > 100.0) percentage = 100.0;
    if (percentage < 0.0) percentage = 0.0;
    
    int filled = (int)((percentage / 100.0) * (width - 2));
    
    // Dibujar etiqueta
    if (label) {
        mvprintw(y, x, "%s:", label);
        x += strlen(label) + 1;
    }
    
    // Dibujar barra
    mvaddch(y, x, '[');
    mvaddch(y, x + width - 1, ']');
    
    // Elegir color basado en porcentaje
    int color = COLOR_SUCCESS;
    if (percentage > 80) color = COLOR_ERROR;
    else if (percentage > 60) color = COLOR_WARNING;
    
    attron(COLOR_PAIR(color));
    for (int i = 0; i < filled; i++) {
        mvaddch(y, x + 1 + i, '#');
    }
    attroff(COLOR_PAIR(color));
    
    // Espacios vacíos
    for (int i = filled; i < width - 2; i++) {
        mvaddch(y, x + 1 + i, ' ');
    }
    
    // Porcentaje
    mvprintw(y, x + width + 1, "%.1f%%", percentage);
}

// Dibujar encabezado
void draw_header(void) {
    attron(COLOR_PAIR(COLOR_INFO));
    mvprintw(0, 0, "┌─ NLX v1.0 - Network Linux Extension");
    for (int i = 35; i < COLS - 1; i++) {
        mvaddch(0, i, '-');
    }
    mvaddch(0, COLS - 1, '┐');
    attroff(COLOR_PAIR(COLOR_INFO));
}

// Dibujar pie de página
void draw_footer(void) {
    int y = LINES - 1;
    attron(COLOR_PAIR(COLOR_INFO));
    mvprintw(y, 0, "└");
    for (int i = 1; i < COLS - 1; i++) {
        mvaddch(y, i, '-');
    }
    mvaddch(y, COLS - 1, '┘');
    
    // Comandos disponibles
    mvprintw(y - 1, 2, "[Q] Salir  [R] Actualizar  [1] Ancho de banda  [2] Conexiones  [3] Interfaces");
    attroff(COLOR_PAIR(COLOR_INFO));
}

// Dibujar sección de ancho de banda
void draw_bandwidth_section(void) {
    draw_box(2, 2, 8, COLS - 4, "Ancho de Banda");
    
    // Obtener interfaz si no está definida
    if (!current_interface) {
        int interface_count;
        char** interfaces = get_available_interfaces(&interface_count);
        if (interfaces && interface_count > 0) {
            for (int i = 0; i < interface_count; i++) {
                if (is_interface_active(interfaces[i])) {
                    current_interface = malloc(strlen(interfaces[i]) + 1);
                    strcpy(current_interface, interfaces[i]);
                    break;
                }
            }
            // Liberar memoria
            for (int i = 0; i < interface_count; i++) {
                free(interfaces[i]);
            }
            free(interfaces);
        }
    }
    
    if (current_interface) {
        // Obtener estadísticas actuales
        previous_stats = current_stats;
        current_stats = collect_network_stats(current_interface);
        
        // Calcular velocidades si tenemos datos previos
        if (previous_stats.timestamp > 0) {
            double time_diff = 1.0; // Asumimos 1 segundo entre actualizaciones
            calculate_speeds(&current_stats, &previous_stats, time_diff);
        }
        
        // Mostrar interfaz
        mvprintw(3, 4, "Interfaz: %s", current_interface);
        
        // Mostrar velocidades
        attron(COLOR_PAIR(COLOR_SUCCESS));
        mvprintw(4, 4, "Descarga: %s", format_speed(current_stats.rx_speed));
        mvprintw(5, 4, "Subida:   %s", format_speed(current_stats.tx_speed));
        mvprintw(6, 4, "Total:    %s", format_speed(current_stats.total_speed));
        attroff(COLOR_PAIR(COLOR_SUCCESS));
        
        // Mostrar bytes totales
        mvprintw(7, 4, "Bytes recibidos: %s", format_bytes(current_stats.rx_bytes));
        mvprintw(8, 4, "Bytes enviados:  %s", format_bytes(current_stats.tx_bytes));
    } else {
        mvprintw(3, 4, "No se encontró interfaz activa");
    }
}

// Dibujar sección de conexiones
void draw_connections_section(void) {
    draw_box(11, 2, 6, COLS - 4, "Conexiones Activas");
    
    connections_count = get_connection_count();
    processes_count = get_active_processes();
    
    mvprintw(12, 4, "Conexiones TCP: %d", connections_count);
    mvprintw(13, 4, "Procesos activos: %d", processes_count);
    mvprintw(14, 4, "Proceso actual: %s (PID %d)", get_process_name(getpid()), getpid());
}

// Dibujar sección de interfaces
void draw_interfaces_section(void) {
    draw_box(18, 2, 4, COLS - 4, "Interfaces de Red");
    
    int interface_count;
    char** interfaces = get_available_interfaces(&interface_count);
    
    if (interfaces && interface_count > 0) {
        mvprintw(19, 4, "Interfaces encontradas: %d", interface_count);
        
        // Mostrar primera interfaz activa
        for (int i = 0; i < interface_count && i < 1; i++) {
            int is_active = is_interface_active(interfaces[i]);
            attron(COLOR_PAIR(is_active ? COLOR_SUCCESS : COLOR_WARNING));
            mvprintw(20, 4, "%s: %s", interfaces[i], is_active ? "activa" : "inactiva");
            attroff(COLOR_PAIR(is_active ? COLOR_SUCCESS : COLOR_WARNING));
        }
        
        // Liberar memoria
        for (int i = 0; i < interface_count; i++) {
            free(interfaces[i]);
        }
        free(interfaces);
    } else {
        mvprintw(19, 4, "No se encontraron interfaces");
    }
}

// Función principal de la interfaz TUI
void run_tui(void) {
    init_ui();
    
    int ch;
    while (1) {
        clear();
        
        // Dibujar interfaz
        draw_header();
        draw_bandwidth_section();
        draw_connections_section();
        draw_interfaces_section();
        draw_footer();
        
        refresh();
        
        // Manejar input
        ch = getch();
        if (ch == 'q' || ch == 'Q') {
            break;
        }
        else if (ch == 'r' || ch == 'R') {
            // Actualizar datos
            continue;
        }
        
        // Esperar un poco
        sleep(1);  // 1 segundo entre actualizaciones
    }
    
    cleanup_ui();
}

// Funciones placeholder para completar la API
void draw_stats_section(void) {
    // TODO: Implementar sección de estadísticas
}

void update_bandwidth_data(void) {
    // TODO: Implementar actualización de datos de ancho de banda
}

void update_connections_data(void) {
    // TODO: Implementar actualización de datos de conexiones
}

void update_interfaces_data(void) {
    // TODO: Implementar actualización de datos de interfaces
}

void draw_table_header(int y, int x, const char* headers[], int num_headers) {
    // TODO: Implementar encabezado de tabla
}

void draw_table_row(int y, int x, const char* values[], int num_values) {
    // TODO: Implementar fila de tabla
} 