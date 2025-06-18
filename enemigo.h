#ifndef Enemigo_h
#define Enemigo_h

#include <locale.h>
#include <ncursesw/ncurses.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdlib>  // para los sonidos
#include <stdlib.h>
#include <time.h>

#include "Disparo.h"

extern int copia_mapa[10][12];
extern int jugador_y;
extern int jugador_x;

extern int totem_x;
extern int totem_y;


class Enemigo
{
protected: 
	int contador_enemigo = 0;
	int velocidad_enemigo = 5;

	int velocidad_disparo = 6;
	int contador_disparo = 0;

	int posicion_x;
	int posicion_y;
	int direccion; // 1=este, 2=oeste, 3=norte, 4=sur

	bool vivo = true;
	
	std::vector<std::vector<std::wstring>> sprite;
	
    // Función para que el enemigo pueda desviarse si hay una ruta disponible
    void verificarYDesviar() {
        int direcciones_perpendiculares[2];
        
        if (direccion == 1 || direccion == 2) { // Si va Este u Oeste
            direcciones_perpendiculares[0] = 3; // Norte
            direcciones_perpendiculares[1] = 4; // Sur
        } else { // Si va Norte o Sur
            direcciones_perpendiculares[0] = 1; // Este
            direcciones_perpendiculares[1] = 2; // Oeste
        }
        // Verificar si alguna ruta está libre
        for (int i = 0; i < 2; i++) {
            int dir_prueba = direcciones_perpendiculares[i];
            int prueba_x = posicion_x;
            int prueba_y = posicion_y;
            
            // Calcular nueva posición en esa dirección
            switch(dir_prueba) {
                case 1: prueba_x++; break; // Este
                case 2: prueba_x--; break; // Oeste
                case 3: prueba_y--; break; // Norte
                case 4: prueba_y++; break; // Sur
            }
            
            // Verificar si esa posición está libre
            if (prueba_x >= 0 && prueba_x < 12 && prueba_y >= 0 && prueba_y < 10) {
                if (copia_mapa[prueba_y][prueba_x] != 1 && copia_mapa[prueba_y][prueba_x] != 2) {
                    // Hay un 50% de probabilidad de desviarse hacia esta dirección libre
                    if (rand() % 2 == 0) {
                        direccion = dir_prueba;
                        return; // Solo se desvía hacia una dirección
                    }
                }
            }
        }
    }
	
public:

    Enemigo(int x, int y) {
        posicion_x = x;
        posicion_y = y;
        direccion = (rand() % 4) + 1;
        
	}  
	
	virtual ~Enemigo() {}
	
    int getPosX() const { 
        return posicion_x; 
    }
    int getPosY() const { 
        return posicion_y; 
    }
    bool getVivo() const {
        return vivo;
    }
    std::vector<std::vector<std::wstring>> getSprite(){
        return sprite;
    }
    int getDireccion() const{
        return direccion;
    }
    
    void mover(){
		contador_enemigo++;
		        
		// Solo moverse cada 'velocidad_enemigo' frames
		if (contador_enemigo < velocidad_enemigo || !vivo) {
		    return;
		}
		contador_enemigo = 0;
		
		// Verificar posibilidad de desviarse (40% de probabilidad)
		if (rand() % 100 < 40) {
		    verificarYDesviar();
		}
		
		int nueva_posicion_x = posicion_x;
		int nueva_posicion_y = posicion_y;
		
		// Moverse en la dirección actual
		switch(direccion) {
		    case 1: // Este (derecha)
		        nueva_posicion_x++;
		        break;
		    case 2: // Oeste (izquierda)
		        nueva_posicion_x--;
		        break;
		    case 3: // Norte (arriba)
		        nueva_posicion_y--;
		        break;
		    case 4: // Sur (abajo)
		        nueva_posicion_y++;
		        break;
		}
		
		// Verificar si puede moverse (no hay obstáculos y está dentro de límites)
		bool puede_moverse = true;
		
		if (nueva_posicion_x < 0 || nueva_posicion_x >= 12 || 
		    nueva_posicion_y < 0 || nueva_posicion_y >= 10) {
		    puede_moverse = false;
		}
		else if (copia_mapa[nueva_posicion_y][nueva_posicion_x] == 1 || 
		    copia_mapa[nueva_posicion_y][nueva_posicion_x] == 2 || copia_mapa[nueva_posicion_y][nueva_posicion_x] == 3 || (nueva_posicion_y == jugador_y && nueva_posicion_x==jugador_x)) {
		    puede_moverse = false;
		}
		
		if (puede_moverse) {
		    posicion_x = nueva_posicion_x;
		    posicion_y = nueva_posicion_y;
		} else {
		    // Si no puede moverse, cambiar a una dirección aleatoria
		    direccion = (rand() % 4) + 1;
		}
	}
	
	virtual void disparar(WINDOW* ventana)= 0;
    
    void morir(){
        vivo = false;
        posicion_x = -10;
        posicion_y = -10;
    }
    
    virtual int get_impactos_destruccion () const
    {
    	return 0;
    }
    
    virtual void nuevo_impacto()
    {
    }
};

#endif
