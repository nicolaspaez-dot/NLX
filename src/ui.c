#include "ui.h"
#include "collector.h"
#include "renderer.h"
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

// Variables globales para gráficos
static GraphData bandwidth_graph;
static TableData connections_table;
static int graph_initialized = 0;

// Inicializar ncurses
void init_ui(void) {
    initscr();              // Inicializar pantalla
    noecho();               // No mostrar teclas presionadas
    curs_set(0);            // Ocultar cursor
    keypad(stdscr, TRUE);   // Habilitar teclas especiales
    cbreak();               // Modo cbreak para input inmediato
    nodelay(stdscr, TRUE);  // getch() no bloquea
    
    // Configuraciones adicionales para evitar parpadeo
    leaveok(stdscr, TRUE);  // No mover cursor al final de operaciones
    scrollok(stdscr, FALSE); // Deshabilitar scroll automático
    idlok(stdscr, FALSE);   // No usar líneas de inserción/eliminación
    
    setup_colors();
    
    // Inicializar gráficos
    init_bandwidth_graph(&bandwidth_graph, "Ancho de Banda");
    bandwidth_graph.max_values = 200; // Aumentar capacidad para más datos
    init_connections_table(&connections_table);
    graph_initialized = 1;
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
    attron(COLOR_PAIR(COLOR_INFO) | A_BOLD);
    mvprintw(0, 0, "+");
    for (int i = 1; i < COLS - 1; i++) {
        mvaddch(0, i, '-');
    }
    mvaddch(0, COLS - 1, '+');
    
    // Título centrado
    int title_x = (COLS - 35) / 2;
    mvprintw(0, title_x, " NLX v1.0 - Network Linux Extension ");
    attroff(COLOR_PAIR(COLOR_INFO) | A_BOLD);
}

// Dibujar pie de página
void draw_footer(void) {
    int y = LINES - 2;
    
    // Línea separadora
    attron(COLOR_PAIR(COLOR_INFO));
    mvaddch(y, 0, '+');
    for (int i = 1; i < COLS - 1; i++) {
        mvaddch(y, i, '-');
    }
    mvaddch(y, COLS - 1, '+');
    attroff(COLOR_PAIR(COLOR_INFO));
    
    // Comandos disponibles - CENTRADOS
    y = LINES - 1;
    attron(COLOR_PAIR(COLOR_INFO));
    
    // Calcular posición centrada para los comandos
    const char* commands = "[Q] Salir  [R] Actualizar  [1] Ancho de banda  [2] Conexiones  [3] Interfaces";
    int commands_len = strlen(commands);
    int start_x = (COLS - commands_len) / 2;
    
    // Asegurar que no se salga de los bordes
    if (start_x < 0) start_x = 0;
    if (start_x + commands_len > COLS) start_x = COLS - commands_len;
    
    mvprintw(y, start_x, "%s", commands);
    attroff(COLOR_PAIR(COLOR_INFO));
}

// Dibujar sección de ancho de banda
void draw_bandwidth_section(void) {
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
            
            // Agregar datos al gráfico
            if (graph_initialized) {
                add_bandwidth_data(&bandwidth_graph, current_stats.total_speed);
            }
        } else {
            // Si no hay datos previos, agregar algunos datos de ejemplo para que se vea el gráfico
            if (graph_initialized) {
                add_bandwidth_data(&bandwidth_graph, 2.5); // 2.5 MB/s de ejemplo
                add_bandwidth_data(&bandwidth_graph, 1.8);
                add_bandwidth_data(&bandwidth_graph, 3.2);
                add_bandwidth_data(&bandwidth_graph, 2.1);
                add_bandwidth_data(&bandwidth_graph, 4.0);
            }
        }
        
        // ========================================
        // SECCIÓN SUPERIOR: GRÁFICOS
        // ========================================
        
        // Título de la sección
        attron(COLOR_PAIR(COLOR_INFO) | A_BOLD);
        mvprintw(1, 2, "=== MONITOREO DE RED ===");
        attroff(COLOR_PAIR(COLOR_INFO) | A_BOLD);
        
        // Gráfico de ancho de banda (lado izquierdo)
        if (graph_initialized) {
            // Dibujar caja del gráfico usando todo el ancho disponible
            int graph_width = COLS - 4; // Todo el ancho menos los bordes
            draw_box(2, 2, 8, graph_width, "Ancho de Banda en Tiempo Real");
            
            // Dibujar gráfico simple de barras
            int bars_width = graph_width - 4; // Ancho menos los bordes internos
            int graph_height = 4;
            double max_speed = bandwidth_graph.max_value > 0 ? bandwidth_graph.max_value : 10.0;
            
            // Dibujar barras del gráfico
            for (int i = 0; i < bars_width; i++) {
                int data_index = (bandwidth_graph.current_index - bars_width + i + bandwidth_graph.max_values) % bandwidth_graph.max_values;
                double value = bandwidth_graph.values[data_index];
                
                if (max_speed > 0) {
                    int bar_height = (int)((value / max_speed) * graph_height);
                    if (bar_height > graph_height) bar_height = graph_height;
                    
                    // Color según el valor
                    if (value > max_speed * 0.8) {
                        attron(COLOR_PAIR(COLOR_ERROR));
                    } else if (value > max_speed * 0.5) {
                        attron(COLOR_PAIR(COLOR_WARNING));
                    } else {
                        attron(COLOR_PAIR(COLOR_SUCCESS));
                    }
                    
                    // Dibujar barra
                    for (int j = 0; j < bar_height; j++) {
                        mvaddch(6 - j, 4 + i, '#');
                    }
                    attroff(COLOR_PAIR(COLOR_ERROR) | COLOR_PAIR(COLOR_WARNING) | COLOR_PAIR(COLOR_SUCCESS));
                }
            }
            
            // Mostrar valor máximo
            mvprintw(8, 4, "Max: %.1f MB/s", max_speed);
        }
        
        // ========================================
        // SECCIÓN MEDIA: INFORMACIÓN DE INTERFAZ
        // ========================================
        
        int info_width = COLS - 4; // Todo el ancho menos los bordes
        draw_box(11, 2, 6, info_width, "Información de Interfaz");
        
        // Información de la interfaz
        attron(COLOR_PAIR(COLOR_INFO));
        mvprintw(12, 4, "Interfaz: %s", current_interface);
        attroff(COLOR_PAIR(COLOR_INFO));
        
        // IP local con color rojo
        char* interface_ip = get_interface_ip(current_interface);
        if (interface_ip) {
            attron(COLOR_PAIR(COLOR_ERROR) | A_BOLD);
            mvprintw(12, 35, "IP Local: %s", interface_ip);
            attroff(COLOR_PAIR(COLOR_ERROR) | A_BOLD);
            free(interface_ip);
        }
        
        // Bytes totales
        mvprintw(13, 4, "Bytes recibidos: %s", format_bytes(current_stats.rx_bytes));
        mvprintw(13, 35, "Bytes enviados: %s", format_bytes(current_stats.tx_bytes));
        
        // Paquetes
        mvprintw(14, 4, "Paquetes recibidos: %lu", current_stats.rx_packets);
        mvprintw(14, 35, "Paquetes enviados: %lu", current_stats.tx_packets);
        
        // Velocidades actuales destacadas
        attron(COLOR_PAIR(COLOR_INFO) | A_BOLD);
        mvprintw(15, 4, "Velocidad de DESCARGA actual: %s", format_speed(current_stats.rx_speed));
        attroff(COLOR_PAIR(COLOR_INFO) | A_BOLD);
        
        attron(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
        mvprintw(16, 4, "Velocidad de SUBIDA actual:   %s", format_speed(current_stats.tx_speed));
        attroff(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
        
    } else {
        mvprintw(3, 4, "No se encontró interfaz activa");
    }
}

// Dibujar sección de conexiones
void draw_connections_section(void) {
    connections_count = get_connection_count();
    processes_count = get_active_processes();
    
    // ========================================
    // SECCIÓN DE CONEXIONES ACTIVAS
    // ========================================
    
    int conn_width = COLS - 4; // Todo el ancho menos los bordes
    draw_box(18, 2, 9, conn_width, "Conexiones Activas");
    
    // Headers de la tabla
    attron(COLOR_PAIR(COLOR_INFO) | A_BOLD);
    mvprintw(19, 4, "Puerto");
    mvprintw(19, 12, "Estado");
    mvprintw(19, 20, "Proceso");
    mvprintw(19, 40, "IP Remota");
    mvprintw(19, 55, "Bytes");
    attroff(COLOR_PAIR(COLOR_INFO) | A_BOLD);
    
    // Línea separadora
    for (int i = 4; i < COLS - 6; i++) {
        mvaddch(20, i, '-');
    }
    
    // Datos de conexiones (ejemplo por ahora)
    mvprintw(21, 4, "80");
    mvprintw(21, 12, "LISTEN");
    attron(COLOR_PAIR(COLOR_SUCCESS));
    mvprintw(21, 20, "nginx");
    attroff(COLOR_PAIR(COLOR_SUCCESS));
    mvprintw(21, 40, "0.0.0.0");
    mvprintw(21, 55, "2.1M");
    
    mvprintw(22, 4, "443");
    mvprintw(22, 12, "LISTEN");
    attron(COLOR_PAIR(COLOR_SUCCESS));
    mvprintw(22, 20, "nginx");
    attroff(COLOR_PAIR(COLOR_SUCCESS));
    mvprintw(22, 40, "0.0.0.0");
    mvprintw(22, 55, "1.8M");
    
    mvprintw(23, 4, "8080");
    mvprintw(23, 12, "ESTAB");
    attron(COLOR_PAIR(COLOR_INFO));
    mvprintw(23, 20, "node");
    attroff(COLOR_PAIR(COLOR_INFO));
    mvprintw(23, 40, "192.168.1.5");
    mvprintw(23, 55, "950K");
    
    mvprintw(24, 4, "22");
    mvprintw(24, 12, "ESTAB");
    attron(COLOR_PAIR(COLOR_WARNING));
    mvprintw(24, 20, "ssh");
    attroff(COLOR_PAIR(COLOR_WARNING));
    mvprintw(24, 40, "192.168.1.10");
    mvprintw(24, 55, "45K");
    
    mvprintw(25, 4, "53");
    mvprintw(25, 12, "ESTAB");
    attron(COLOR_PAIR(COLOR_INFO));
    mvprintw(25, 20, "systemd-resolve");
    attroff(COLOR_PAIR(COLOR_INFO));
    mvprintw(25, 40, "8.8.8.8");
    mvprintw(25, 55, "12K");
    
    // Estadísticas en la parte inferior
    attron(COLOR_PAIR(COLOR_INFO));
    mvprintw(26, 4, "Total conexiones TCP: %d", connections_count);
    mvprintw(26, 35, "Procesos activos: %d", processes_count);
    attroff(COLOR_PAIR(COLOR_INFO));
}

// Dibujar sección de interfaces
void draw_interfaces_section(void) {
    // ========================================
    // SECCIÓN DE INTERFACES DE RED
    // ========================================
    
    int iface_width = COLS - 4; // Todo el ancho menos los bordes
    draw_box(28, 2, 6, iface_width, "Interfaces de Red");
    
    int interface_count;
    char** interfaces = get_available_interfaces(&interface_count);
    
    if (interfaces && interface_count > 0) {
        mvprintw(29, 4, "Interfaces encontradas: %d", interface_count);
        
        // Mostrar interfaces activas
        int line_count = 0;
        for (int i = 0; i < interface_count && i < 3; i++) {
            int is_active = is_interface_active(interfaces[i]);
            if (is_active) {
                attron(COLOR_PAIR(COLOR_SUCCESS));
                mvprintw(30 + line_count, 4, "* %s = activa", interfaces[i]);
                attroff(COLOR_PAIR(COLOR_SUCCESS));
            } else {
                attron(COLOR_PAIR(COLOR_WARNING));
                mvprintw(30 + line_count, 4, "o %s = inactiva", interfaces[i]);
                attroff(COLOR_PAIR(COLOR_WARNING));
            }
            line_count++;
        }
        
        // Información adicional
        mvprintw(33, 4, "* = Activa  o = Inactiva");
        
        // Liberar memoria
        for (int i = 0; i < interface_count; i++) {
            free(interfaces[i]);
        }
        free(interfaces);
    } else {
        mvprintw(29, 4, "No se encontraron interfaces de red");
    }
}

// Función principal de la interfaz TUI
void run_tui(void) {
    init_ui();
    
    int ch;
    while (1) {
        // Limpiar pantalla de manera más eficiente
        werase(stdscr);
        
        // Dibujar interfaz
        draw_header();
        draw_bandwidth_section();
        draw_connections_section();
        draw_interfaces_section();
        draw_footer();
        
        // Usar doble buffer para actualización sin parpadeo
        wnoutrefresh(stdscr);
        doupdate();
        
        // Manejar input
        ch = getch();
        if (ch == 'q' || ch == 'Q') {
            break;
        }
        else if (ch == 'r' || ch == 'R') {
            // Actualizar datos
            continue;
        }
        
        
        sleep(1);  
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