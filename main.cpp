#include <locale.h>
#include <ncursesw/ncurses.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdlib>  // para los sonidos
#include <stdlib.h>
#include <time.h>
#include <memory> 
#include <ctime>
#include <chrono>
#include <unistd.h>

#include "enemigo_normal.h"
#include "enemigo_nivel_dos.h"
#include "enemigo_nivel_tres.h"
#include "enemigo_ultimo.h"

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
  {1,1,0,1,1,1,0,0,0,0,1,0},
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
  {1,0,1,0,0,0,0,0,0,1,1,0},
  {0,0,0,0,0,2,0,0,0,0,0,0},
  {0,0,0,0,2,2,2,0,0,0,0,0},
  {0,0,0,0,0,2,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,1,0,0,0,0,0,0,1,1,2},
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

class Tank {
    private:
        std::vector<std::vector<std::wstring>> sprites_normal;
        std::vector<std::vector<std::wstring>> sprites_nivel_dos;
        int direccion = 0;
        bool vivo = true;
    public:
        Tank()
            : sprites_normal{
                {L" ║", L"║█║", L" ━"},   // arriba
                {L" ━ ", L"║█║", L" ║"},  // abajo
                {L"══ ", L"██═", L"══ "}, // derecha
                {L" ══", L"═██", L" ══"}   // izquierda
            }
        {
        	sprites_nivel_dos =
        	{
                {L" ║", L"║█║", L" ━"},   // arriba
                {L" ━ ", L"║█║", L" ║"},  // abajo
                {L"══ ", L"██═", L"══ "}, // derecha
                {L" ══", L"═██", L" ══"}   // izquierda
        	};
        }
        
        void set_direccion(int dir)
        {
            if(dir>=0 && dir <4)
            {
                this->direccion = dir;
            }   	
        }
        std::vector<std::vector<std::wstring>> getSprite_normal(){
            return sprites_normal;
        }
        
        int get_altura() const
        {
            return 3;
        }
        
        int get_ancho() const 
        {
            return 3;
        }

        int get_direccion() {
            return direccion;
        }

};

// Crear enemigos

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
                    wattron(win, COLOR_PAIR(3));
                    for(int i=0; i<3; i++)
		            {
                        mvwaddwstr(win, wy+i, wx, L"###");
                    }
                    wattroff(win, COLOR_PAIR(3));
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
                    mvwaddwstr(win, wy+1, wx, L"#❤#");
		    mvwaddwstr(win, wy+2, wx, L"###");
                    wattroff(win, COLOR_PAIR(7));
                    break;
                case 7:
                    wattron(win, COLOR_PAIR(3));
	            mvwaddwstr(win, wy, wx, L"###");
                    mvwaddwstr(win, wy+1, wx, L"#🛡#");
                    mvwaddwstr(win, wy+2, wx, L"###");
                    wattroff(win, COLOR_PAIR(3));
                    break;
                case 8:
                    wattron(win, COLOR_PAIR(8));
		    mvwaddwstr(win, wy, wx, L"###");
                    mvwaddwstr(win, wy+1, wx, L"#❄#");
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
        if(nivelActual!=5 || d.fromPlayer){

            wattron(win, COLOR_PAIR(3));
            mvwaddwstr(win, wy, wx, L" ⬤");
            wattroff(win, COLOR_PAIR(3));
        }
        
        else{
            for (auto& enemigo : enemigosActuales) {
                int dir = enemigo->getDireccion() - 1;
                if(dir==3 || dir == 2)
                {
                    wattron(win, COLOR_PAIR(3));
                    mvwaddwstr(win, wy, wx, L" █");
                    wattroff(win, COLOR_PAIR(3));
                }
                else 
                {
                    wattron(win, COLOR_PAIR(3));
                    mvwaddwstr(win, wy, wx, L" *");
                    wattroff(win, COLOR_PAIR(3));

                }
            }
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
    const char* line1  = "                  ██                      ";
    const char* line2  = "                 █████                 ";
    const char* line3  = "               ███████                 ";
    const char* line4  = "             ████████                  ";
    const char* line5  = "           ████████        █          ";
    const char* line6  = "         ████████        █████        ";
    const char* line7  = "        ████████       █████████      ";
    const char* line8  = "          ████████   ████████████     ";
    const char* line9  = "            ███████████████████████    ";
    const char* line10 = "  ████        ███████████    ████████ ";
    const char* line11 = "████████       ████████        ████████";
    const char* line12 = "  ████████    ███████████        ████  ";
    const char* line13 = "    ███████████████████████            ";
    const char* line14 = "      ████████████   ████████          ";
    const char* line15 = "       █████████       ████████        ";
    const char* line16 = "         █████        ████████         ";
    const char* line17 = "           █        ████████           ";
    const char* line18 = "                  ████████             ";
    const char* line19 = "                ████████               ";
    const char* line20 = "                 █████                 ";
    const char* line21 = "                   █                   ";

    int tempX = centerHorizontal - 19;

    mvprintw(19, tempX, "%s", line1);
    mvprintw(18, tempX, "%s", line2);
    mvprintw(17, tempX, "%s", line3);
    mvprintw(18, tempX, "%s", line4);
    mvprintw(19, tempX, "%s", line5);
    mvprintw(20, tempX, "%s", line6);
    mvprintw(21, tempX, "%s", line7);
    mvprintw(22, tempX, "%s", line8);
    mvprintw(23, tempX, "%s", line9);
    mvprintw(24, tempX, "%s", line10);
    mvprintw(25, tempX, "%s", line11);
    mvprintw(26, tempX, "%s", line12);
    mvprintw(27, tempX, "%s", line13);
    mvprintw(28, tempX, "%s", line14);
    mvprintw(29, tempX, "%s", line15);
    mvprintw(30, tempX, "%s", line16);
    mvprintw(31, tempX, "%s", line17);
    mvprintw(32, tempX, "%s", line18);
    mvprintw(33, tempX, "%s", line19);
    mvprintw(34, tempX, "%s", line20);
    mvprintw(35, tempX, "%s", line21);
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
    refresh();
}

void printPantallaPuntajes(int centerVertical, int centerHorizontal, bool nuevoPuntaje) {
    int text1X = centerHorizontal - 46;
    int text1Y = centerVertical - 26;

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
    wrefresh(rankingWIN);      //Dibuja el mapa apartir de la matriz
}

void obtenerCaracteres(char *text,int y, int x) {
    echo();  // Mostrar lo que el usuario escribe
    curs_set(1);  // Hacer visible el cursor
    int ch;
    int len = 0;

    mvprintw(y-1, x, "%s", "Ingrese su nombre (Solo tres caracteres)");
    while (len < 3) {
        ch = mvwgetch(stdscr, y, x + len);  // Lee carácter por carácter

        if (ch == ERR || ch == '\n') continue;  // Ignora errores o Enter
        if (ch == 127 || ch == KEY_BACKSPACE) {  // Backspace
            if (len > 0) {
                len--;
                mvwaddch(stdscr, y, x + len, ' ');  // Borra el carácter
            }
            continue;
        }

        text[len] = ch;
        len++;
    }
    text[len] = '\0';  // Termina la cadena
    curs_set(0);  // Oculta el cursor
}

void procesoPuntajes(int centerVertical, int centerHorizontal) {
    bool nuevoPuntaje = false;
    if (puntajes.size() > 0) {        
        int i = 0;
        while(!nuevoPuntaje) {
            if (puntajes[i] == puntaje) {
                puntajes.insert(puntajes.begin(), i, puntaje);
            }
        }
    } else {
        puntajes.push_back(puntaje);            
        nuevoPuntaje = true;
    }    

    char name[3];
    obtenerCaracteres(name, 20, 20);
    std::string nameString(name);
    nombres.push_back(nameString);    
    clear();
    printPantallaPuntajes(centerVertical, centerHorizontal, nuevoPuntaje);
}

void renderizadoJuego(WINDOW* gamewin) {   
        srand(time(NULL));
    setlocale(LC_ALL, "");
    initscr();
    keypad(stdscr, TRUE);

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
    init_pair(8, COLOR_CYAN,   -1);  // par 8 = celeste

    int map_height = 10;
    int map_width = 12;
    int win_height = map_height * 3+2;
    int win_width = map_width * 3+2;

    //Dimensiones de la pantalla
    int start_y = (LINES - win_height) / 2;
    int start_x = (COLS - win_width) / 2;

    gamewin = newwin(win_height, win_width, start_y, start_x);
    int info_width = 20;
    WINDOW* info_win = newwin(win_height, info_width, start_y, start_x + win_width+1);

    int centerVertical = (LINES / 2);
    int centerHorizontal = (COLS / 2);

    printPantallaInicial(centerHorizontal);

    refresh();

    timeout(-1);
    getch();
    timeout(100);
    clear();

    int ch;
    bool ejecutando = true;    
    int cantEnemigos = 4;
    int cantNiveles = 2;
    
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 12; j++) {
            copia_mapa[i][j] = mapa[i][j];
        }
    }   

    

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
			
            if (nivelActual == 1 && cantEnemigos == 0) {
                cantEnemigos = 4;
                jugador_x=9;
                jugador_y=8;
                totem_x = 6;
                totem_y = 8;
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
            
               if (nivelActual == 2 && cantEnemigos == 0) {
                cantEnemigos = 4;
                nivelActual++;
                jugador_x=0;
                jugador_y=2;
                totem_x = 4;
                totem_y = 0;
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

            //Nivel 4
            if (nivelActual == 3 && cantEnemigos == 0) {
                cantEnemigos = 4;
                nivelActual++;
                totem_x = 6;
                totem_y = 9;
                jugador_x=0;
                jugador_y=0;
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
            
            //Nivel 5
            if (nivelActual == 4 && cantEnemigos == 0) {
                cantEnemigos = 1;
                nivelActual++;
                jugador_x=11;
                jugador_y=8;
                disparos.clear();
                for (int i = 0; i < 10; i++) {
                    for (int j = 0; j < 12; j++) {
                        copia_mapa[i][j] = mapa5[i][j];
                    }
                } 
                enemigosActuales.emplace_back(enemigos[++indice_actual].get());
            }


		    
		    werase(gamewin);        
		    box(gamewin, 0, 0);         //Dibuja los bordes de la pantalla
            int prev_enemy_y;
            int prev_enemy_x; 
            
            if(!congelar)
            {
                for (auto& enemigo : enemigosActuales) {
                enemigo->mover();
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

		    dibujar_mapa(gamewin, copia_mapa);
            //Lineas para dibujar un cuadro con la información de la partida
            werase(info_win);
            box(info_win, 0, 0);  // Borde de la ventana de info
            mvwprintw(info_win, 2, 2, "Nivel:   %d", nivelActual);
            mvwprintw(info_win, 4, 2, "Puntaje: %d", puntaje);

            //mvwprintw(info_win, 6, 2, "Vidas:   %d", vidas);
            mvwprintw(info_win, 6, 2, "Vidas: ");
            for(int i= 0; i<vidas; i++){
                waddwstr(info_win, L" ♥");

            }


            wrefresh(info_win);      //Dibuja el mapa apartir de la matriz
		    wrefresh(gamewin);          //refresh para el renderizado

		    ch = getch();
		    if (ch == 'q') 
            {
                juego=false;
                break;
            }       //Opción para salir del juego

		    int new_y = jugador_y;
		    int new_x = jugador_x;

            if(!congelar)
            {
		        for (auto& enemigo : enemigosActuales) {
                enemigo->disparar(gamewin);
                }
            }
		    //Para determinar los movimientos del jugador 
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
                        disparos.push_back({ jugador_y, jugador_x, dx, dy, true });
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

		    for (auto& d : disparos) {
		        d.y += d.dy;
		        d.x += d.dx;
		    }
		    
		    

		    //Determina dónde irán los disparos, también maneja el efecto de destrucción de los bloques
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
		        }
		        else if (it->fromPlayer)
                {   
                    for (auto& enemigo : enemigosActuales) {
                        if (std::abs(it->y - enemigo->getPosY()) + std::abs(it->x - enemigo->getPosX()) <= 1 ){
                            if(nivelActual == 5 && enemigo->get_impactos_destruccion()>0)
                            {
                                system("aplay -q sounds/disparoBlindado.wav &");
                                enemigo->nuevo_impacto();
                                eliminar = true;
                                break;
                                
                            }
                            copia_mapa[it->y][it->x] = 0;
                            system("aplay -q sounds/muerte.wav &");
                            efecto_destruccion(gamewin, it->y, it->x);
                            enemigo->morir();
                            puntaje += 10;                            
                            if (indice_actual < enemigos.size()-1) {

                            } else {
                                ejecutando = false;
                                ganar = true;
                                break;
                            }
                            cantEnemigos--;
                            if (cantEnemigos >= 2){
                                enemigosActuales.emplace_back(enemigos[++indice_actual].get());
                            }
                            eliminar = true;
                            break;
                        }
                    }
                    if(copia_mapa[it->y][it->x] == 3)
                    {
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
		        //LO que sucede si se impacta jugador
		        else if (it->y == jugador_y && it->x == jugador_x)
		        {   
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
					jugador_y = -10;
		            jugador_x = -10;
		            eliminar = true;
		            ejecutando = false;
                    ganar = false;
                    system("aplay -q sounds/muerte.wav &");
                    break;

		           }
		           else 
		           {	if(nivelActual==3)
			   	{
					jugador_y = 0;
					jugador_x = 0;
					continue;
				}
		           	jugador_y = 9;
		           	jugador_x = 8;
                    		continue;
		           }
		            
		        }
		        //Lo que sucede si se impacta la base
		        else if(copia_mapa[it->y][it->x] == 3)
		        {
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
    wrefresh(gamewin);

    clear();
    refresh();

    if (ganar) {
        printPantallaGanar(centerVertical,centerHorizontal);
    } else {
        printPantallaFinal(centerVertical, centerHorizontal);
    }

    while (true) {
        auto ch = getch();

        if (ch == 'q') {
            break;
        }
    }

    clear();
    procesoPuntajes(centerVertical, centerHorizontal);

    while (true) {
        auto ch = getch();

        if (ch == 'c') {
            break;
        }
    }
    reiniciar_valores();   
    clear();
    wrefresh(gamewin);
}

int main() {
    WINDOW* gamewin;

    while (juego) {
        renderizadoJuego(gamewin);
    }

    delwin(gamewin);
    endwin();
    return 0;
}
