#include <locale.h>
#include <ncursesw/ncurses.h>
#include <string.h>
#include <vector>
#include <cstdlib>  // para los sonidos
#include <time.h>
#include <memory> 
#include <chrono>
#include <unistd.h>

#include "enemigo_normal.h"
#include "enemigo_nivel_dos.h"
#include "enemigo_nivel_tres.h"
#include "enemigo_ultimo.h"
#include "tank.h"

#include "Disparo.h"

using Clock = std::chrono::system_clock;
using namespace std::chrono;

// Matriz donde se representan los elementos del mapa 
//(los especifico en un switch más adelante moppss)

//6 = Vida - 7 = Escudo - 8 = Congelar
int mapa[10][12] = {
  {0,1,0,1,0,1,0,1,0,1,0,0},
  {0,1,0,0,0,0,0,1,0,1,0,0},
  {0,0,0,1,6,1,0,0,0,0,0,0},
  {1,1,0,0,0,0,0,1,1,0,1,0},
  {0,0,0,1,0,1,0,0,0,0,0,0},
  {0,1,0,1,1,1,0,1,0,1,0,0},
  {0,0,0,1,0,1,0,1,0,1,1,0},
  {0,1,0,0,0,0,0,1,0,1,0,0},
  {0,1,0,1,1,1,0,1,0,1,1,0},
  {0,0,0,1,3,1,0,0,0,0,0,0},
};

int mapa2[10][12] = {
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,1,0,1,0,1,0,1,1,0},
  {0,1,0,1,0,1,7,1,0,1,0,0},
  {0,1,0,1,0,0,0,0,0,1,0,0},
  {0,0,0,0,0,1,0,1,0,0,0,0},
  {1,0,1,1,0,0,0,0,1,1,0,1},
  {0,0,0,0,0,1,0,0,0,0,0,0},
  {0,1,1,1,0,1,1,1,0,0,1,0},
  {0,0,1,0,0,1,3,1,0,0,1,0},
  {0,0,0,0,0,1,1,1,0,0,0,0},
};

int mapa3[10][12] = {
  {0,0,0,1,3,1,0,0,0,0,1,0},
  {2,1,0,1,1,1,0,0,0,0,1,0},
  {0,0,0,0,1,0,0,1,0,0,1,0},
  {0,1,1,0,0,0,1,1,0,0,0,0},
  {0,1,8,0,0,0,0,1,0,0,0,1},
  {0,0,0,0,1,1,0,1,0,0,0,1},
  {0,1,1,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,0,1,0,0,1,1,0},
  {0,1,0,0,0,1,1,0,0,1,0,0},
  {0,0,0,0,0,0,0,0,0,0,1,0},
};

int mapa4[10][12] = {
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,1,0,1,0,1,0,1,0,1},
  {0,1,0,1,0,1,7,1,0,1,0,1},
  {0,1,0,1,0,1,0,1,0,1,0,1},
  {0,0,0,0,0,0,0,0,0,1,0,0},
  {1,8,0,0,0,0,0,0,0,0,0,6},
  {0,1,0,1,0,0,0,0,1,1,0,0},
  {0,1,0,1,0,0,0,0,0,0,1,0},
  {0,1,0,1,0,1,1,1,0,1,1,0},
  {0,0,0,0,0,1,3,1,0,0,0,0},
};

int mapa5[10][12] = {
  {0,0,1,0,0,0,0,0,0,1,0,0},
  {0,0,1,0,0,0,0,0,0,2,0,0},
  {1,0,1,0,1,1,1,1,0,1,1,0},
  {0,0,0,0,0,2,0,0,0,0,0,0},
  {1,1,1,0,2,2,2,0,0,1,1,1},
  {0,0,0,0,0,2,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {2,1,1,0,1,1,1,1,0,1,1,2},
  {0,0,1,0,0,0,0,0,0,7,0,0},
  {0,0,6,0,0,0,0,0,0,0,0,0},
};

//Posición inicial del bichito
int jugador_y = 9, jugador_x = 8;

// Última dirreción a la que se movió
int direccion = KEY_UP;

//vidas
int vidas = 4;

// puntaje
int puntaje = 0;
int puntaje_nivel = 0;

//Nivel actual
int nivelActual = 1;   

// Mejores puntajesAdd commentMore actions
std::vector<int> puntajes;
std::vector<std::string> nombres;

//Bandera escudo y tiempos
bool escudo = false;

//Bandera congelado
bool congelar = false;

//Banderas de disparo y ganar
bool disparo = true;
bool ganar =  false;

//Booleando que indica que se sigue en juego
bool juego = true;
bool salirJuego = false;
bool enPrincipal = true;

std::chrono::steady_clock::time_point escudo_inicio;
std::chrono::steady_clock::time_point congelar_inicio;
constexpr double COOLDOWN = 0.5; 
std::chrono::steady_clock::time_point lastShot =
    std::chrono::steady_clock::now()
  - std::chrono::duration_cast<std::chrono::steady_clock::duration>(
        std::chrono::duration<double>(COOLDOWN)
    );

double tiempo_disparo;
double tiempo_transcurrido;

int copia_mapa[10][12];

//Totem pos
int totem_x = 4;
int totem_y = 9;

std::vector<std::unique_ptr<Enemigo>> enemigos;
std::vector<Enemigo*> enemigosActuales;

size_t indice_actual = 0;                // cuál enemigo estamos usando
        
std::vector<Disparo> disparos;

// Aquí recibe las coordenadas de donde se quiere destruir la estructura
void destruccion_estructura(int y, int x) {
    if (y >= 0 && y < 10 && x >= 0 && x < 12 && copia_mapa[y][x] == 1 ) {
        copia_mapa[y][x] = 0;
        system("aplay -q sounds/destruccion.wav &");
    }
    if (y >= 0 && y < 10 && x >= 0 && x < 12 && copia_mapa[y][x] == 6 ) {
        copia_mapa[y][x] = 0;
        system("aplay -q sounds/coin.wav &");
        vidas++;
        return;
    }
    if(y >= 0 && y < 10 && x >= 0 && x < 12 && copia_mapa[y][x] == 7)
    {   
        copia_mapa[y][x] = 0;
        system("aplay -q sounds/shield.wav &");
        escudo = true;
        escudo_inicio = std::chrono::steady_clock::now();
        return;
    }
        if(y >= 0 && y < 10 && x >= 0 && x < 12 && copia_mapa[y][x] == 8)
    {   
        copia_mapa[y][x] = 0;
        system("aplay -q sounds/freeze.wav &");
       
        congelar = true;
        congelar_inicio = std::chrono::steady_clock::now();
        return;
    }
    }

void reiniciar_valores()
{
    jugador_y = 9, jugador_x = 8;

    // Última dirreción a la que se movió
    direccion = KEY_UP;

    //vidas
    vidas = 4;

    // puntaje
    puntaje = 0;

    //puntaje por nivel
    puntaje_nivel= 0;

    //Nivel actual
    nivelActual = 1;    

    //Bandera escudo y tiempos
    escudo = false;

    //Bandera congelado
    congelar = false;

    //Banderas de disparo y ganar
    disparo = true;
    ganar =  false;
    juego=true;
    salirJuego = false;
    enPrincipal = true;

    totem_x = 4;
    totem_y = 9;
    indice_actual = 0;                // cuál enemigo estamos usando
    enemigosActuales.clear();

    enemigos.clear();
    disparos.clear();
}

void efecto_destruccion(WINDOW* win, int y, int x) {
    wattron(win, COLOR_PAIR(5));
    mvwaddwstr(win, y*3+1, x*3+1, L"**");
    wattroff(win, COLOR_PAIR(5));
    wrefresh(win);
    napms(200);
}

Tank tank;

// Aquí es donde metí la matriz para dibujar el mapa
void dibujar_mapa(WINDOW* win, int copia_mapa[10][12]) {
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 12; ++x) {
            int wy = y * 3 + 1;
            int wx = x * 3 + 1;
             //Interpretación de los caracteres según la matriz
             if (y == jugador_y && x == jugador_x) {
                if(escudo)
                {
                    wattron(win, COLOR_PAIR(0));
                    mvwaddwstr(win, wy+1, wx, tank.getSprite_normal()[tank.get_direccion()][1].c_str());
                    mvwaddwstr(win, wy, wx, tank.getSprite_normal()[tank.get_direccion()][0].c_str());
                    mvwaddwstr(win, wy+2, wx, tank.getSprite_normal()[tank.get_direccion()][2].c_str()); 
                    wattroff(win, COLOR_PAIR(0));
                    continue;
                }
                wattron(win, COLOR_PAIR(2));
                mvwaddwstr(win, wy+1, wx, tank.getSprite_normal()[tank.get_direccion()][1].c_str());
                mvwaddwstr(win, wy, wx, tank.getSprite_normal()[tank.get_direccion()][0].c_str());
                mvwaddwstr(win, wy+2, wx, tank.getSprite_normal()[tank.get_direccion()][2].c_str()); 
                wattroff(win, COLOR_PAIR(2));
                continue;
            }
            else{
                bool enemigoEncontrado = false;
                for (auto& enemigo : enemigosActuales) {
                    if (y == enemigo->getPosY() && x == enemigo->getPosX() && enemigo->getVivo()){
                        wattron(win, COLOR_PAIR(6));
                        std::vector<std::vector<std::wstring>> s = enemigo->getSprite();
                        int dir = enemigo->getDireccion() - 1;
                        mvwaddwstr(win, wy+1, wx, s[dir][1].c_str());
                        mvwaddwstr(win, wy, wx, s[dir][0].c_str());
                        mvwaddwstr(win, wy+2, wx, s[dir][2].c_str());
                        wattroff(win, COLOR_PAIR(6));
                        enemigoEncontrado = true;
                        break; // Salir del bucle una vez encontrado el enemigo
                    }
                }
            if (!enemigoEncontrado) {
            switch (copia_mapa[y][x]) {
                case 1:
                    wattron(win, COLOR_PAIR(1));
                    for(int i=0; i<3; i++)
		            {
                        mvwaddwstr(win, wy+i, wx, L"###");
                    }
                    wattroff(win, COLOR_PAIR(1));
                    break;
                case 2:
                    wattron(win, COLOR_PAIR(5));
                    for(int i=0; i<3; i++)
		            {
                        mvwaddwstr(win, wy+i, wx, L"###");
                    }
                    wattroff(win, COLOR_PAIR(5));
                    break;
                case 3:
                    wattron(win, COLOR_PAIR(3));
                    mvwaddwstr(win, wy, wx, L" ▄ ");
                    mvwaddwstr(win, wy+1, wx, L"║█║" );
                    mvwaddwstr(win, wy+2, wx, L" █ " );
                    wattroff(win, COLOR_PAIR(3));
                    break;
                case 6:
                    wattron(win, COLOR_PAIR(7));        
		    mvwaddwstr(win, wy, wx, L"###");
                    mvwaddwstr(win, wy+1, wx, L"###");
		    mvwaddwstr(win, wy+2, wx, L"###");
                    wattroff(win, COLOR_PAIR(7));
                    break;
                case 7:
                    wattron(win, COLOR_PAIR(3));
	            mvwaddwstr(win, wy, wx, L"###");
                    mvwaddwstr(win, wy+1, wx, L"###");
                    mvwaddwstr(win, wy+2, wx, L"###");
                    wattroff(win, COLOR_PAIR(3));
                    break;
                case 8:
                    wattron(win, COLOR_PAIR(8));
		    mvwaddwstr(win, wy, wx, L"###");
                    mvwaddwstr(win, wy+1, wx, L"###");
		    mvwaddwstr(win, wy+2, wx, L"###");
                    wattroff(win, COLOR_PAIR(8));
                    break; 
                    
                default:
                    wattron(win, COLOR_PAIR(4));
                    for(int i=0; i<3; i++)
		            {
                        mvwaddwstr(win, wy+i, wx, L"   ");
                    }
                    wattroff(win, COLOR_PAIR(4));
                    break;
            }
        }}}
    }
	
    for (const auto& d : disparos) {
        int wy = d.y * 3 + 1;
        int wx = d.x * 3 + 1;
        if(d.fromPlayer || !d.fromBoss){

            wattron(win, COLOR_PAIR(3));
            mvwaddwstr(win, wy, wx, L" ⬤");
            wattroff(win, COLOR_PAIR(3));
        }
        
        else{
            wattron(win, COLOR_PAIR(3));
            mvwaddwstr(win, wy, wx, L" *");
            wattroff(win, COLOR_PAIR(3));                
            }
        }
        
    
    }

void printPantallaInicial(int centerHorizontal) {   
    int text1X = centerHorizontal - 65;

    attron(COLOR_PAIR(3));
    const char* msg = "░▒▓███████▓▒░ ░▒▓██████▓▒░▒▓████████▓▒░▒▓████████▓▒░▒▓█▓▒░      ░▒▓████████▓▒░       ░▒▓██████▓▒░░▒▓█▓▒░▒▓████████▓▒░▒▓█▓▒░░▒▓█▓▒░";
    mvprintw(2, text1X, "%s", msg);
    const char* msg1 = "░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░      ░▒▓█▓▒░   ░▒▓█▓▒░      ░▒▓█▓▒░             ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░  ░▒▓█▓▒░   ░▒▓█▓▒░░▒▓█▓▒░";
    mvprintw(3, text1X, "%s", msg1);
    const char* msg2 = "░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░      ░▒▓█▓▒░   ░▒▓█▓▒░      ░▒▓█▓▒░             ░▒▓█▓▒░      ░▒▓█▓▒░  ░▒▓█▓▒░   ░▒▓█▓▒░░▒▓█▓▒░";
    mvprintw(4, text1X, "%s", msg2);
    const char* msg3 = "░▒▓███████▓▒░░▒▓████████▓▒░ ░▒▓█▓▒░      ░▒▓█▓▒░   ░▒▓█▓▒░      ░▒▓██████▓▒░        ░▒▓█▓▒░      ░▒▓█▓▒░  ░▒▓█▓▒░    ░▒▓██████▓▒░ ";
    mvprintw(5, text1X, "%s", msg3);
    const char* msg4 = "░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░      ░▒▓█▓▒░   ░▒▓█▓▒░      ░▒▓█▓▒░             ░▒▓█▓▒░      ░▒▓█▓▒░  ░▒▓█▓▒░      ░▒▓█▓▒░    ";
    mvprintw(6, text1X, "%s", msg4);
    const char* msg5 = "░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░      ░▒▓█▓▒░   ░▒▓█▓▒░      ░▒▓█▓▒░             ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░  ░▒▓█▓▒░      ░▒▓█▓▒░    ";
    mvprintw(7, text1X, "%s", msg5);
    const char* msg6 = "░▒▓███████▓▒░░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░      ░▒▓█▓▒░   ░▒▓████████▓▒░▒▓████████▓▒░       ░▒▓██████▓▒░░▒▓█▓▒░  ░▒▓█▓▒░      ░▒▓█▓▒░    ";
    mvprintw(8, text1X, "%s", msg6);

    int text2X = centerHorizontal - 21;
    const char* msg7 = "-- PRESIONE CUALQUIER TECLA PARA EMPEZAR --";
    mvprintw(12, text2X, "%s", msg7);

    attron(COLOR_PAIR(5));
    const char* line1  = "             █████████████████            ██████████ ";
    const char* line2  = "             █████████████████  █████████████████    ";
    const char* line3  = "             ██████████████████████████              ";
    const char* line4  = "             ███████████████████                     ";
    const char* line5  = "              █               ██                     ";
    const char* line6  = "     ██████████████████████████████████████████      ";
    const char* line7  = "    ████████████████████████████████████████████     ";
    const char* line8  = "  █████████████████████████████████████████████████  ";
    const char* line9  = " ██   █                                       █   ██ ";
    const char* line10 = "██ ███  █████  ██████████  ██████████  ██████  ███ ██";
    const char* line11 = "   ██    ██   █   ████   ██  █████   █   ███    ██   ";
    const char* line12 = "    ██████  █████ ████  █████ ███  █████  ███████    ";
    const char* line13 = "     █████  ████  ████  ████  ████  ████  ██████     ";
    const char* line14 = "      █████      ██████      ██████      ██████      ";
    const char* line15 = "        █████████████████████████████████████        ";

    int tempX = centerHorizontal - 26;

    mvprintw(17, tempX, "%s", line1);
    mvprintw(18, tempX, "%s", line2);
    mvprintw(19, tempX, "%s", line3);
    mvprintw(20, tempX, "%s", line4);
    mvprintw(21, tempX, "%s", line5);
    mvprintw(22, tempX, "%s", line6);
    mvprintw(23, tempX, "%s", line7);
    mvprintw(24, tempX, "%s", line8);
    mvprintw(25, tempX, "%s", line9);
    mvprintw(26, tempX, "%s", line10);
    mvprintw(27, tempX, "%s", line11);
    mvprintw(28, tempX, "%s", line12);
    mvprintw(29, tempX, "%s", line13);
    mvprintw(30, tempX, "%s", line14);
    mvprintw(31, tempX, "%s", line15);
}

void printPantallaNivelCompletado(int centerVertical, int centerHorizontal, int nivel, int puntaje) {
    // Crear ventana centrada
    int alto_menu = 8;
    int ancho_menu = 50;
    int start_y = (centerVertical - alto_menu) / 2;
    int start_x = centerHorizontal - (ancho_menu / 2);
    
    WINDOW* level_win = newwin(alto_menu, ancho_menu, start_y, start_x);
    werase(level_win);
    box(level_win, 0, 0);  

    mvwprintw(level_win, 1, 1, "Nivel completado: %d", nivel);
    mvwprintw(level_win, 2, 1, "Puntaje obtenido en el nivel: %d", puntaje_nivel);
    mvwprintw(level_win, 3, 1, "Puntaje hasta ahora obtenido: %d", puntaje);
    mvwprintw(level_win, 5, 1, "Presiona 'q' continuar...");
    
    
    int ch;
    while ((ch = getch()) != 'q') {
        wrefresh(level_win);
        refresh();
    }

    clear();
    refresh();
}

void printPantallaGanar(int centerVertical, int centerHorizontal) {
    int text1X = centerHorizontal - 16;
    int text1Y = centerVertical - 12;

    // Definir colores para simular transparencia (grises)
    init_pair(1, COLOR_WHITE, -1);     
    init_pair(2, COLOR_YELLOW, -1); 

    attron(COLOR_PAIR(2));
    // Todas las líneas del ASCII art
    std::vector<const char*> figuras = {
        "      ████████████████████      ",
        "      ████████████████████      ",
        "  █████                  █████  ",
        "██ ████                  ████ ██",
        "█ █   ██                ██   █ █",
        "█ █    ██              ██    █ █",
        "█ █     ██            ██     █ █",
        " █ █     ██          ██     █ █ ",
        " ██ ██  █  █        █  █  ██ ██ ",        
        "   █  ████ █        █ ████  █   ",
        "     █████ █        █ █████     ",

        "            ██    ██            ",
        "             ██████             ",
        "             ██████             ",
        "             ██  ██             ",
        "           ██████████           ",
        "        ████████████████        ",
        "        ████████████████        ",
        "        ████████████████        ",
    };

    const char* texto[] = {
        " ██████╗  █████╗ ███╗   ██╗ █████╗ ███████╗████████╗███████╗",
        "██╔════╝ ██╔══██╗████╗  ██║██╔══██╗██╔════╝╚══██╔══╝██╔════╝",
        "██║  ███╗███████║██╔██╗ ██║███████║███████╗   ██║   █████╗  ",
        "██║   ██║██╔══██║██║╚██╗██║██╔══██║╚════██║   ██║   ██╔══╝  ",
        "╚██████╔╝██║  ██║██║ ╚████║██║  ██║███████║   ██║   ███████╗",
        " ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝╚══════╝   ╚═╝   ╚══════╝"
    };

    for (int i = 0; i < figuras.size(); i++) {            
        mvprintw(text1Y++, text1X, "%s", figuras[i]);
    }
    attron(COLOR_PAIR(1));
    refresh();

    int lineas = sizeof(texto) / sizeof(texto[0]);
    int posX = centerHorizontal - 30; 
    int posY = centerVertical + 10; 

    for (int i = 0; i < lineas; i++) {
        mvprintw(posY++, posX, "%s", texto[i]);
        usleep(150000);
        refresh();
    }

    int text2X = centerHorizontal - 15;
    const char* msg7 = "-- Presione Q para continuar --";
    mvprintw(posY+2, text2X, "%s", msg7);

}

void printPantallaFinal(int centerVertical, int centerHorizontal) {
    keypad(stdscr, FALSE);
    int text1X = centerHorizontal - 30;
    int text1Y = centerVertical - 20;

    // Definir colores para simular transparencia (grises)
    init_pair(1, COLOR_WHITE, -1);     
    init_pair(2, COLOR_RED, -1); 

    attron(COLOR_PAIR(2));
    // Todas las líneas del ASCII art
    std::vector<const char*> figuras = {
        "            @@                                              ",
        "          @+:::@                                            ",
        "         @::::::@                            @@:::@@        ",
        "        @@::::::@                           @+::::::@       ",
        "     @@*+@@::::@@                           @:::::::@       ",
        "    @=:::::::::@@     @@@@@@@@@@@            @:::::@@@@@    ",
        "    @::::::::::::@@@#:::::::::::::::%@@     @@::::::::::%@  ",
        "     @#::*@@*::::@::::::::::::::::::::::@@@@:::::::::::::@  ",
        "            @@#@::::@@@@@@@@::::::::::::::@-:::::@::::::#@  ",
        "             @@:::@@@@@@@@@@@@::::::::::::::@::@@  @@@@@    ",
        "            @@:::@@@@@@@@@@@@@:::@@@@@@@-::::@@             ",
        "            @:::@@@@@@@@@@@@@@::@@@@@@@@@@::::@             ",
        "            @::=@@@@@@@@@@@@@@:*@@@@@@@@@@@:::@             ",
        "            @::=@@@@@@@@@@@@@+:@@@@@@@@@@@@:::@             ",
        "            @:::@@@@@@@@@@@@=::=@@@@@@@@@@@:::@             ",
        "            @#:::@@@@@@@@@@:::::@@@@@@@@@@*:::@             ",
        "             @-::::#@@@@-::::::::=@@@@@@@::::%              ",
        "              @:::::::::::::@@::::::::::::::%@              ",
        "       @@@    @@@::::::::::::::::::::::::::@@               ",
        "     @*::::@@+::::#%::::::::::::::::::::++:%@               ",
        "     @:::::::::::-@ @#::%:::::::::::@@@+:::::@@@##@@        ",
        "     @@:::-::::@@     @@@@@@@@@@@+#@   @@:::::::::::@       ",
        "       @@@:::::@                         @:::::::::#@       ",
        "        @@:::::@@                      @@:::::@@@@@@        ",
        "         @@:::#@                       @#:::::*@            ",
        "                                        @+:::=@             ",
        "                                          @@@               "
    };

    const char* texto[] = {
        "██████╗ ███████╗██████╗ ██████╗ ██╗███████╗████████╗███████╗",
        "██╔══██╗██╔════╝██╔══██╗██╔══██╗██║██╔════╝╚══██╔══╝██╔════╝",
        "██████╔╝█████╗  ██████╔╝██║  ██║██║███████╗   ██║   █████╗  ",
        "██╔═══╝ ██╔══╝  ██╔══██╗██║  ██║██║╚════██║   ██║   ██╔══╝  ",
        "██║     ███████╗██║  ██║██████╔╝██║███████║   ██║   ███████╗",
        "╚═╝     ╚══════╝╚═╝  ╚═╝╚═════╝ ╚═╝╚══════╝   ╚═╝   ╚══════╝"
    };

    for (int i = 0; i <= 26; i++) {            
        mvprintw(text1Y++, text1X, "%s", figuras[i]);
    }
    attron(COLOR_PAIR(1));
    refresh();

    int lineas = sizeof(texto) / sizeof(texto[0]);
    int posX = centerHorizontal - 30; 
    int posY = centerVertical + 10; 

    // Simular aumento de opacidad en varios pasos
    for (int i = 0; i < lineas; i++) {
        mvprintw(posY++, posX, "%s", texto[i]);
        usleep(150000);
        refresh();
    }

    int text2X = centerHorizontal - 15;
    const char* msg7 = "-- Presione Q para continuar --";
    mvprintw(posY+2, text2X, "%s", msg7);

    refresh();
}

void printPantallaPuntajes(int centerVertical, int centerHorizontal) {
    int text1X = centerHorizontal - 46;
    int text1Y = centerVertical - 20;

    // Definir colores para simular transparencia (grises)
    init_pair(1, COLOR_WHITE, -1); 

    attron(COLOR_PAIR(1));

    const char* texto[] = {
        "    ██████   ██████ ██████████       █████    ███████    ███████████   ██████████  █████████          ",
        "   ░░██████ ██████ ░░███░░░░░█      ░░███   ███░░░░░███ ░░███░░░░░███ ░░███░░░░░█ ███░░░░░███         ",
        "    ░███░█████░███  ░███  █ ░        ░███  ███     ░░███ ░███    ░███  ░███  █ ░ ░███    ░░░          ",
        "    ░███░░███ ░███  ░██████          ░███ ░███      ░███ ░██████████   ░██████   ░░█████████          ",
        "    ░███ ░░░  ░███  ░███░░█          ░███ ░███      ░███ ░███░░░░░███  ░███░░█    ░░░░░░░░███         ",
        "    ░███      ░███  ░███ ░   █ ███   ░███ ░░███     ███  ░███    ░███  ░███ ░   █ ███    ░███         ",
        "    █████     █████ ██████████░░████████   ░░░███████░   █████   █████ ██████████░░█████████          ",
        "   ░░░░░     ░░░░░ ░░░░░░░░░░  ░░░░░░░░      ░░░░░░░    ░░░░░   ░░░░░ ░░░░░░░░░░  ░░░░░░░░░           ",
        " ███████████  █████  █████ ██████   █████ ███████████   █████████         █████ ██████████  █████████ ",
        "░░███░░░░░███░░███  ░░███ ░░██████ ░░███ ░█░░░███░░░█  ███░░░░░███       ░░███ ░░███░░░░░█ ███░░░░░███",
        " ░███    ░███ ░███   ░███  ░███░███ ░███ ░   ░███  ░  ░███    ░███        ░███  ░███  █ ░ ░███    ░░░ ",
        " ░██████████  ░███   ░███  ░███░░███░███     ░███     ░███████████        ░███  ░██████   ░░█████████ ",
        " ░███░░░░░░   ░███   ░███  ░███ ░░██████     ░███     ░███░░░░░███        ░███  ░███░░█    ░░░░░░░░███",
        " ░███         ░███   ░███  ░███  ░░█████     ░███     ░███    ░███  ███   ░███  ░███ ░   █ ███    ░███",
        " █████        ░░████████   █████  ░░█████    █████    █████   █████░░████████   ██████████░░█████████ ",
        "░░░░░          ░░░░░░░░   ░░░░░    ░░░░░    ░░░░░    ░░░░░   ░░░░░  ░░░░░░░░   ░░░░░░░░░░  ░░░░░░░░░  "
    };

    for (int i = 0; i < 16; i++) {            
        mvprintw(text1Y++, text1X, "%s", texto[i]);
    }
    refresh();

    WINDOW* rankingWIN = newwin(puntajes.size()+4, 35, text1Y+2, text1X+31);
    werase(rankingWIN);
    box(rankingWIN, 0, 0);  

    text1Y += 3;
    if (puntajes.size() > 0) {
        mvprintw(text1Y, text1X+33, "%s", "     NOMBRE\tPUNTAJE");
        text1Y += 2;
        for (int i = 0; i < puntajes.size(); i++) {
            std::string texto = "  " + std::to_string(i+1) + ". " + nombres[i] + "\t" + std::to_string(puntajes[i]);
            mvprintw(text1Y++, text1X+33, "%s", texto.c_str());
        }
    } else {
        for (int i = 0; i <= 5; i++) {
            mvprintw(text1Y++, text1X, "%s", ". HOLAAAS");
        }
    }

    text1Y += 5;
    std::string text = "Tu puntaje en esta partida: " + std::to_string(puntaje);
    mvprintw(text1Y++, text1X+34, "%s", text.c_str());

    int text2X = centerHorizontal - 15;
    const char* msg7 = "-- Presione Q para continuar --";
    mvprintw(text1Y+2, text2X+2, "%s", msg7);

    wrefresh(rankingWIN);      //Dibuja el mapa apartir de la matriz
}

int printPantallaVolverJugar(int centerVertical, int centerHorizontal) {
    // Crear ventana centrada
    int alto_menu = 12;
    int ancho_menu = 50;
    int start_y = (centerVertical - alto_menu) / 2;
    int start_x = centerHorizontal - (ancho_menu / 2);
    
    WINDOW* ventana_menu = newwin(alto_menu, ancho_menu, start_y, start_x);
    
    std::vector<std::string> opciones = {"Sí, volver a jugar", "No, salir del juego"};
    int seleccion = 0;
    int tecla;
    
    while(true) {
        // Limpiar y dibujar el cuadro
        werase(ventana_menu);
        box(ventana_menu, 0, 0);
        
        // Título principal
        std::string titulo = "¿VOLVER A JUGAR?";
        int pos_titulo = (ancho_menu - titulo.length()) / 2;
        mvwprintw(ventana_menu, 2, pos_titulo, "%s", titulo.c_str());
        
        // Línea decorativa
        for(int i = 1; i < ancho_menu - 1; i++) {
            mvwprintw(ventana_menu, 4, i, "-");
        }
        
        // Mostrar opciones
        for(int i = 0; i < opciones.size(); i++) {
            int pos_y = 6 + (i * 2);
            int pos_x = (ancho_menu - opciones[i].length()) / 2;
            
            if(i == seleccion) {
                wattron(ventana_menu, A_REVERSE | A_BOLD);
                mvwprintw(ventana_menu, pos_y, pos_x - 2, "> %s <", opciones[i].c_str());
                wattroff(ventana_menu, A_REVERSE | A_BOLD);
            } else {
                mvwprintw(ventana_menu, pos_y, pos_x, "%s", opciones[i].c_str());
            }
        }
        
        // Instrucciones
        std::string instrucciones = "Usa ↑↓ para navegar, ENTER para seleccionar";
        int pos_instr = (ancho_menu / 2) - (instrucciones.length() / 2) + 2;
        mvwprintw(ventana_menu, 10, pos_instr, "%s", instrucciones.c_str());
        
        wrefresh(ventana_menu);
        
        // Manejar input
        tecla = getch();
        
        switch(tecla) {
            case KEY_UP:
                seleccion = (seleccion - 1 + opciones.size()) % opciones.size();
                break;
                
            case KEY_DOWN:
                seleccion = (seleccion + 1) % opciones.size();
                break;
                
            case 10: // ENTER
            case 13: // ENTER alternativo
                delwin(ventana_menu);
                return seleccion; // 0 = Sí, 1 = No
                
            case 27: // ESC para cancelar
                delwin(ventana_menu);
                return -1;
        }
    }
}

void obtenerCaracteres(char *text, int y, int x) {
    noecho();
    curs_set(1);
    keypad(stdscr, TRUE);
    
    mvprintw(y-1, x, "Ingrese su nombre (Solo tres letras):");
    move(y, x);
    refresh();
    
    int ch;
    int len = 0;
    
    while (len < 3) {
        ch = getch();
        
        // Filtrar cualquier cosa que no sea ASCII imprimible normal
        if (ch < 0 || ch > 255) {
            continue;  // Teclas especiales de ncurses
        }
        
        // Manejar backspace
        if (ch == 127 || ch == 8) {  // DEL o BS
            if (len > 0) {
                len--;
                move(y, x + len);
                addch(' ');
                move(y, x + len);
                refresh();
            }
        }
        // Solo letras ASCII
        else if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            text[len] = ch;
            addch(ch);
            refresh();
            len++;
        }
    }
    
    mvprintw(y+5, x, "Presiona Enter para continuar...");
    refresh();
    
    while ((ch = getch()) != '\n' && ch != '\r') {
        // Espera Enter
    }
    
    text[3] = '\0';
    
    move(y+1, x);
    clrtoeol();
    
    curs_set(0);
}

void mostrarCuadroIngresarNombre(char *nombre) {
    int alto_ventana, ancho_ventana;
    getmaxyx(stdscr, alto_ventana, ancho_ventana);
    
    // Crear ventana centrada
    int alto_cuadro = 10;
    int ancho_cuadro = 60;
    int start_y = (alto_ventana - alto_cuadro) / 2;
    int start_x = (ancho_ventana - ancho_cuadro) / 2;
    
    WINDOW* ventana_input = newwin(alto_cuadro, ancho_cuadro, start_y, start_x);
    
    // Dibujar el cuadro
    box(ventana_input, 0, 0);
    
    // Título principal
    std::string titulo = "INGRESO DE NOMBRE";
    int pos_titulo = (ancho_cuadro - titulo.length()) / 2;
    mvwprintw(ventana_input, 2, pos_titulo, "%s", titulo.c_str());
    
    // Línea decorativa
    for(int i = 1; i < ancho_cuadro - 1; i++) {
        mvwprintw(ventana_input, 3, i, "-");
    }
    
    // Área de entrada con borde
    for(int i = 5; i < 8; i++) {
        mvwprintw(ventana_input, i, 10, "|");
        mvwprintw(ventana_input, i, ancho_cuadro - 11, "|");
    }
    for(int i = 11; i < ancho_cuadro - 11; i++) {
        mvwprintw(ventana_input, 5, i, "-");
        mvwprintw(ventana_input, 7, i, "-");
    }
    mvwprintw(ventana_input, 5, 10, "+");
    mvwprintw(ventana_input, 5, ancho_cuadro - 11, "+");
    mvwprintw(ventana_input, 7, 10, "+");
    mvwprintw(ventana_input, 7, ancho_cuadro - 11, "+");
    
    wrefresh(ventana_input);
    
    // Llamar a la función obtenerCaracteres con las coordenadas correctas
    // Convertir coordenadas de ventana a coordenadas de pantalla
    int input_y = start_y + 6;  // Dentro del cuadro de entrada
    int input_x = start_x + 12; // Posición donde escribir
    
    obtenerCaracteres(nombre, input_y, input_x);
    
    // Limpiar la ventana
    delwin(ventana_input);
    clear();
    refresh();
}

void procesoPuntajes(int centerVertical, int centerHorizontal) {
    const int MAX_PUNTAJES = 5;
    char name[3];

    clear();
    refresh();

    mostrarCuadroIngresarNombre(name);
    std::string nameString(name);
    
    // Si el vector está vacío o no está lleno, agrega
    if (puntajes.size() < MAX_PUNTAJES) {
        puntajes.push_back(puntaje);
        nombres.push_back(nameString);
    } else // Si está lleno, solo agregar si es mejor que el peor puntaje
        if (puntaje > puntajes.back()) {
            puntajes.push_back(puntaje);
            nombres.push_back(nameString);
    }
    
    // Crear índices para ordenar ambos vectores juntos
    std::vector<int> indices(puntajes.size());
    for (int i = 0; i < indices.size(); i++) {
        indices[i] = i;
    }
    
    // Ordenar índices basado en puntajes (mayor a menor)
    std::sort(indices.begin(), indices.end(), [&](int a, int b) {
        return puntajes[a] > puntajes[b];
    });
    
    // Crear vectores temporales ordenados
    std::vector<int> puntajesOrdenados;
    std::vector<std::string> nombresOrdenados;
    
    for (int i : indices) {
        puntajesOrdenados.push_back(puntajes[i]);
        nombresOrdenados.push_back(nombres[i]);
    }
    
    // Reemplazar vectores originales
    puntajes = puntajesOrdenados;
    nombres = nombresOrdenados;
    
    // Mantener top 5
    if (puntajes.size() > MAX_PUNTAJES) {
        puntajes.resize(MAX_PUNTAJES);
        nombres.resize(MAX_PUNTAJES);
    }

    clear();
    printPantallaPuntajes(centerVertical, centerHorizontal);
}

void cargarRecursos(WINDOW* &gamewin, int &op, int &win_height, int &win_width, int &start_y, int &start_x, 
                   int centerVertical, int centerHorizontal, bool &ejecutando, int &cantEnemigos, WINDOW* &info_win) {
    enemigosActuales.clear();
    enemigos.emplace_back(std::make_unique<enemigo_normal>(0, 0));
    enemigos.emplace_back(std::make_unique<enemigo_normal>(10, 0));
    enemigos.emplace_back(std::make_unique<enemigo_normal>(0, 0));
    enemigos.emplace_back(std::make_unique<enemigo_normal>(10, 0));
    
    enemigos.emplace_back(std::make_unique<enemigo_nivel_dos>(0, 0));
    enemigos.emplace_back(std::make_unique<enemigo_nivel_dos>(0, 0));
    enemigos.emplace_back(std::make_unique<enemigo_nivel_dos>(0, 0));
    enemigos.emplace_back(std::make_unique<enemigo_nivel_dos>(0, 0));

    enemigos.emplace_back(std::make_unique<enemigo_nivel_tres>(0, 9));
    enemigos.emplace_back(std::make_unique<enemigo_nivel_tres>(9, 9));
    enemigos.emplace_back(std::make_unique<enemigo_nivel_tres>(0, 9));
    enemigos.emplace_back(std::make_unique<enemigo_nivel_tres>(0, 9));
    
    enemigos.emplace_back(std::make_unique<enemigo_normal>(9, 0));
    enemigos.emplace_back(std::make_unique<enemigo_normal>(9, 0));
    enemigos.emplace_back(std::make_unique<enemigo_nivel_dos>(0, 0));
    enemigos.emplace_back(std::make_unique<enemigo_nivel_tres>(9, 0));
    
    enemigos.emplace_back(std::make_unique<enemigo_normal>(9, 0));
    enemigos.emplace_back(std::make_unique<enemigo_normal>(1, 0));
    enemigos.emplace_back(std::make_unique<enemigo_ultimo>(0, 0));

    enemigosActuales.emplace_back(enemigos[indice_actual].get());
    enemigosActuales.emplace_back(enemigos[++indice_actual].get());
    
    start_color();
    use_default_colors();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    //Colores definidos para los diferentes elementos de la pantalla
    // el -1 significa sin fondo
    init_pair(1, COLOR_BLUE, -1);
    init_pair(2, COLOR_YELLOW, -1);
    init_pair(3, COLOR_WHITE, -1);
    init_pair(4, COLOR_BLACK, -1);
    init_pair(5, COLOR_RED, -1);
    init_pair(6, COLOR_MAGENTA, -1); // Enemigo
    init_pair(7, COLOR_GREEN, -1);   //vida
    if (COLORS >= 256) {
    init_pair(8, 14 /*Celeste claro*/, -1);
	}
    int map_height = 10;
    int map_width = 12;
    win_height = map_height * 3+2;
    win_width = map_width * 3+2;

    //Dimensiones de la pantalla
    start_y = (LINES - win_height) / 2;
    start_x = (COLS - win_width) / 2;

    gamewin = newwin(win_height, win_width, start_y, start_x);
    int info_width = 28;
    info_win = newwin(win_height, info_width, start_y, start_x + win_width+1);

    refresh();
    timeout(-1);
    if (op == 1) {
        getch();
    }
    timeout(100);
    clear();

    ejecutando = true;
    cantEnemigos = 4;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 12; j++) {
            copia_mapa[i][j] = mapa[i][j];
        }
    } 
}

void cargarSiguienteNivel(int &cantEnemigos, int &jugador_x, int &jugador_y, int &totem_x, int &totem_y) {
    // Carga nivel 2
    int x, y;
    if (nivelActual == 1 && cantEnemigos == 0) {
        puntaje_nivel = puntaje_nivel*vidas;
        puntaje += puntaje_nivel;
        getmaxyx(stdscr, y, x);
        clear();
        printPantallaNivelCompletado(y/2, x/2, nivelActual, puntaje);
        puntaje_nivel = 0;
        cantEnemigos = 4;
        jugador_x=9;
        jugador_y=8;
        totem_x = 6;
        totem_y = 8;
    	escudo = false;
    	congelar = false;
        nivelActual++;
        for (auto it = disparos.begin(); it != disparos.end();) {
            it = disparos.erase(it);
        }
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 12; j++) {
                copia_mapa[i][j] = mapa2[i][j];
            }
        }
        enemigosActuales.emplace_back(enemigos[++indice_actual].get());
        enemigosActuales.emplace_back(enemigos[++indice_actual].get()); 
    }
       
     // Carga nivel 3
    if (nivelActual == 2 && cantEnemigos == 0) {        
        puntaje_nivel = puntaje_nivel*vidas;
        puntaje += puntaje_nivel;
        getmaxyx(stdscr, y, x);
        clear();
        printPantallaNivelCompletado(y/2, x/2, nivelActual, puntaje);
        cantEnemigos = 4;
        nivelActual++;
        puntaje_nivel = 0;
        jugador_x=0;
        jugador_y=0;
        totem_x = 4;
        totem_y = 0;
 	escudo = false;
    	congelar = false;
        for (auto it = disparos.begin(); it != disparos.end();) {
            it = disparos.erase(it);
        }
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 12; j++) {
                copia_mapa[i][j] = mapa3[i][j];
            }
        } 
        enemigosActuales.emplace_back(enemigos[++indice_actual].get());
        enemigosActuales.emplace_back(enemigos[++indice_actual].get());
    }

     // Carga nivel 4
    if (nivelActual == 3 && cantEnemigos == 0) {
        puntaje_nivel = puntaje_nivel*vidas;
        puntaje += puntaje_nivel;
        getmaxyx(stdscr, y, x);
        clear();
        printPantallaNivelCompletado(y/2, x/2, nivelActual, puntaje);
        cantEnemigos = 4;
        nivelActual++;
        puntaje_nivel = 0;
        totem_x = 6;
        totem_y = 9;
        jugador_x=8;
        jugador_y=9;
	escudo = false;
    	congelar = false;
        for (auto it = disparos.begin(); it != disparos.end();) {
            it = disparos.erase(it);
        }
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 12; j++) {
                copia_mapa[i][j] = mapa4[i][j];
            }
        } 
        enemigosActuales.emplace_back(enemigos[++indice_actual].get());
        enemigosActuales.emplace_back(enemigos[++indice_actual].get());
    }
        
     // Carga nivel 5
    if (nivelActual == 4 && cantEnemigos == 0) {
        puntaje_nivel = puntaje_nivel*vidas;
        puntaje += puntaje_nivel;
        getmaxyx(stdscr, y, x);
        clear();
    	printPantallaNivelCompletado(y/2, x/2, nivelActual, puntaje);
        puntaje_nivel = 0;
        cantEnemigos = 3;
        nivelActual++;
        jugador_x=11;
        jugador_y=8;
	escudo = false;
    	congelar = false;
        disparos.clear();
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 12; j++) {
                copia_mapa[i][j] = mapa5[i][j];
            }
        } 
        enemigosActuales.emplace_back(enemigos[++indice_actual].get());
        enemigosActuales.emplace_back(enemigos[++indice_actual].get());
        enemigosActuales.emplace_back(enemigos[++indice_actual].get());
    } 
}

void manejoEnemigos(WINDOW *gamewin) {
        if(!congelar)
        {
            for (auto& enemigo : enemigosActuales) {
            enemigo->mover();
            }
            for (auto& enemigo : enemigosActuales) {
                enemigo->disparar(gamewin);
            }
        }
        else{
            auto ahora = std::chrono::steady_clock::now();
            auto duracion = std::chrono::duration_cast<std::chrono::seconds>(ahora - congelar_inicio);
            if (duracion.count() >= 2) {
                congelar = false;
            }                
        }
        
        if(escudo)
        {  
            auto ahora = std::chrono::steady_clock::now();
            auto duracion = std::chrono::duration_cast<std::chrono::seconds>(ahora - escudo_inicio);
            if (duracion.count() >= 5) {
                escudo = false;
            }
        }
    
}

void entradaTecleado(int &new_y, int &new_x, int &ch) {
    switch (ch) {
        case KEY_UP:    new_y--; direccion = KEY_UP; tank.set_direccion(0); break;
        case KEY_DOWN:  new_y++; direccion = KEY_DOWN; tank.set_direccion(1); break;
        case KEY_LEFT:  new_x--; direccion = KEY_LEFT; tank.set_direccion(3); break;
        case KEY_RIGHT: new_x++; direccion = KEY_RIGHT; tank.set_direccion(2); break;
        case ' ': 
            auto now = std::chrono::steady_clock::now();
            double elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - lastShot).count();
            if (elapsed >= COOLDOWN) {
                int dy = 0, dx = 0;
                if (direccion == KEY_UP)    dy = -1;
                else if (direccion == KEY_DOWN)  dy =  1;
                else if (direccion == KEY_LEFT)  dx = -1;
                else if (direccion == KEY_RIGHT) dx =  1;
                disparos.push_back({ jugador_y, jugador_x, dx, dy, true, false });
                lastShot = now;
            }
            break;
            
        }         

    //Condicionales para que el jugador no pueda atravesar los bloques
    if (new_y >= 0 && new_y < 10 && new_x >= 0 && new_x < 12) {
        if (copia_mapa[new_y][new_x] != 1 && copia_mapa[new_y][new_x] != 2 && copia_mapa[new_y][new_x] != 3
        && copia_mapa[new_y][new_x] != 6 && copia_mapa[new_y][new_x] != 7 && copia_mapa[new_y][new_x] != 8) {
            for (auto& enemigo : enemigosActuales) {
                if(new_x == enemigo->getPosX() && new_y == enemigo->getPosY()){
                    new_y = jugador_y;
                    new_x = jugador_x;
                }
            }
            jugador_y = new_y;
            jugador_x = new_x;
        }
    }
}

void logicaJuego(int &cantEnemigos, bool &ejecutando) {
    for (auto it = disparos.begin(); it != disparos.end();) {
        bool eliminar = false;
        if (it->x < 0 || it->x >= 12 || it->y < 0 || it->y >= 10) {
            eliminar = true;
        } else if (copia_mapa[it->y][it->x] == 1 || copia_mapa[it->y][it->x] == 2 || copia_mapa[it->y][it->x] == 6 
        ||copia_mapa[it->y][it->x] == 7 ||copia_mapa[it->y][it->x] == 8) {
            destruccion_estructura(it->y, it->x);
            //efecto_destruccion(gamewin, it->y, it->x);
            if(nivelActual!=5 ||(nivelActual==5 && it->fromPlayer))
                eliminar = true;
        } else if (it->fromPlayer) {   
            for (auto& enemigo : enemigosActuales) {
                if (std::abs(it->y - enemigo->getPosY()) + std::abs(it->x - enemigo->getPosX()) <= 1 ){
                    if(nivelActual == 5 && enemigo->get_impactos_destruccion()>1)
                    {
                        system("aplay -q sounds/disparoBlindado.wav &");
                        enemigo->nuevo_impacto();
                        eliminar = true;
                        break;
                        
                    }
                    copia_mapa[it->y][it->x] = 0;
                    system("aplay -q sounds/muerte.wav &");
                    cantEnemigos--;
                    enemigo->morir();

                    if(nivelActual == 1){
                        puntaje_nivel += 10;
                    }
                    if(nivelActual == 2){
                        puntaje_nivel += 20;
                    }
                    if(nivelActual == 3){
                        puntaje_nivel += 30;
                    }
                    if(nivelActual == 4){
                        puntaje_nivel += 40;
                    }
                    if(nivelActual == 5){
                        puntaje_nivel += 50;
                    } 
                                          
                    if (indice_actual < enemigos.size()-1) {

                    } else {
                    if(cantEnemigos ==0)
                    {
                        puntaje += puntaje_nivel;
                        ejecutando = false;
                        ganar = true;
                        break;
                    }
                    }
                    if (cantEnemigos >= 2 && nivelActual!=5){
                        enemigosActuales.emplace_back(enemigos[++indice_actual].get());
                    }
                    eliminar = true;
                    break;
                }
            }
            if(copia_mapa[it->y][it->x] == 3)
            {
                puntaje += puntaje_nivel;
                copia_mapa[it->y][it->x] = 0;
                system("aplay -q sounds/destruccion.wav &");
                vidas=0; 
                //efecto_destruccion(gamewin, it->y, it->x);
                jugador_y = -10;
                jugador_x = -10;
                eliminar = true;
                ejecutando = false;
                break;

            }
        }
        //Lo que sucede si se impacta jugador
        else if (it->y == jugador_y && it->x == jugador_x) {   
            if(escudo)
            {   
                it = disparos.erase(it);                       
                continue;
            }
            copia_mapa[it->y][it->x] = 0;
            system("aplay -q sounds/destruccion.wav &");
            vidas--;
            if(vidas == 0)
            {
            //efecto_destruccion(gamewin, it->y, it->x);
            puntaje += puntaje_nivel;
            jugador_y = -10;
            jugador_x = -10;
            eliminar = true;
            ejecutando = false;
            ganar = false;
            system("aplay -q sounds/muerte.wav &");
            break;
            } else {	
                if(nivelActual==3) {
                    jugador_y = 0;
                    jugador_x = 0;
                    continue;
                }
                jugador_y = 9;
                jugador_x = 8;
                continue;
            }
            
        } else if(copia_mapa[it->y][it->x] == 3) {
            puntaje += puntaje_nivel;
            copia_mapa[it->y][it->x] = 0;
            system("aplay -q sounds/destruccion.wav &");
            vidas=0; 
            //efecto_destruccion(gamewin, it->y, it->x);
            jugador_y = -10;
            jugador_x = -10;
            eliminar = true;
            ejecutando = false;
            break;

        } 

        if (eliminar) {
            it = disparos.erase(it);
            continue;
        } else {
            ++it;
        }
    }
}

void renderizadoJuego(WINDOW* gamewin, int &op) {   
    srand(time(NULL));
    setlocale(LC_ALL, "");
    initscr();
    keypad(stdscr, TRUE);

    WINDOW* info_win;

    int win_height;
    int win_width;
    //Dimensiones de la pantalla
    int start_y;
    int start_x;

    int centerVertical = (LINES / 2);
    int centerHorizontal = (COLS / 2);

    int ch;
    bool ejecutando;    
    int cantEnemigos;

    if (op == 1) {        
        printPantallaInicial(centerHorizontal);
        enPrincipal = false;
    }

    cargarRecursos(gamewin, op, win_height, win_width, start_y, start_x, centerVertical, 
                  centerHorizontal, ejecutando, cantEnemigos, info_win);

	while (ejecutando) {        
        static int last_lines = LINES, last_cols = COLS;
        if (LINES != last_lines || COLS != last_cols) {
            last_lines = LINES;
            last_cols = COLS;
            
            start_y = (LINES - win_height) / 2;
            start_x = (COLS - win_width) / 2;
            
            mvwin(gamewin, start_y, start_x);
            mvwin(info_win, start_y, start_x + win_width + 1);
            touchwin(stdscr);
            redrawwin(stdscr);
        }
			 
        werase(gamewin);        
        box(gamewin, 0, 0);         //Dibuja los bordes de la pantalla

        dibujar_mapa(gamewin, copia_mapa);
        
        manejoEnemigos(gamewin);

        //Lineas para dibujar un cuadro con la información de la partida
        werase(info_win);
        box(info_win, 0, 0);  // Borde de la ventana de info
        mvwprintw(info_win, 2, 2, "Nivel:   %d", nivelActual);
        mvwprintw(info_win, 4, 2, "Puntaje del nivel: %d", puntaje_nivel);
        //mvwprintw(info_win, 6, 2, "Puntaje: %d", puntaje);

        //mvwprintw(info_win, 6, 2, "Vidas:   %d", vidas);
        mvwprintw(info_win, 6, 2, "Vidas: ");
        for(int i= 0; i<vidas; i++){
            waddwstr(info_win, L" ♥");
        }

        wrefresh(info_win);      //Dibuja el mapa apartir de la matriz
        wrefresh(gamewin);       //refresh para el renderizado

        //Opción para salir del juego
        ch = getch();
        if (ch == 27 && enPrincipal == false) 
        {
            salirJuego = true;
            break;
        }       

        int new_y = jugador_y;
        int new_x = jugador_x;

        //Para determinar los movimientos del jugador 
        entradaTecleado(new_y, new_x, ch);

        for (auto& d : disparos) {
            d.y += d.dy;
            d.x += d.dx;
        }    

        //Determina dónde irán los disparos, también maneja el efecto de destrucción de los bloques
        logicaJuego(cantEnemigos, ejecutando);
        cargarSiguienteNivel(cantEnemigos, jugador_x, jugador_y, totem_x, totem_y);
    }

    wrefresh(gamewin);

    clear();
    refresh();

    if (!salirJuego) {
        if (ganar) {
            printPantallaGanar(centerVertical,centerHorizontal);
        } else {
            printPantallaFinal(centerVertical, centerHorizontal);
        }

        ch = 0;
        while ((ch = getch()) != 'q') {};

        clear();
        procesoPuntajes(centerVertical, centerHorizontal);

        ch = 0;
        while ((ch = getch()) != 'q') {};

        
        clear();
        wrefresh(gamewin);

        op = printPantallaVolverJugar(centerVertical, centerHorizontal);
        clear();
        wrefresh(gamewin);
    }
    reiniciar_valores(); 
}

int main() {
    WINDOW* gamewin;
    int op = 1;

    while (juego) {
        renderizadoJuego(gamewin, op);
    }

    endwin();
    return 0;
}
