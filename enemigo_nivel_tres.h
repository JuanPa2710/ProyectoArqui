#ifndef Enemigo_nivel_tres_h
#define Enemigo_nivel_tres_h

#include "enemigo.h"


class enemigo_nivel_tres : public Enemigo
{
public:
	
	enemigo_nivel_tres(int x, int y) : Enemigo(x, y)
	{
		sprite = {
                {L"┅┅ ", L"░░=", L"══ "}, // derecha
                {L" ┅┅", L"=░░", L" ┅┅"},   // izquierda	
                {L" ║", L"⋮░⋮", L" ━"},   // arriba
                {L" ━ ", L"⋮░⋮", L" ║"},  // abajo
            };
            this->velocidad_enemigo = 2;
            this->velocidad_disparo = 8;
       	
	}
	
	void disparar(WINDOW* ventana) override 
	{
	 bool disparoEnemigo = false;

            if (contador_disparo!=velocidad_disparo || !vivo){
                contador_disparo++;
                return;
            }
            contador_disparo = 0;
            int dispararJugador = (rand() % 3) + 1;
            if(posicion_x == jugador_x || posicion_x == totem_x) {
                if(jugador_y - posicion_y < 0 ||totem_y - posicion_y < 0){
                    for (int i = posicion_y; i >= 0; i--){
                        if(copia_mapa[i][posicion_x] == 1 || copia_mapa[i][posicion_x] == 2 || copia_mapa[i][posicion_x] == 6 || copia_mapa[i][posicion_x] == 7 || copia_mapa[i][posicion_x] == 8){
                            break;
                        }else if(i == jugador_y || i== totem_y){
                            disparoEnemigo = true;
                            direccion = 3;
                            break;
                        }
                    }
                }else{
                    for (int i = posicion_y; i <= 11; i++){
                        if(copia_mapa[i][posicion_x] == 1 || copia_mapa[i][posicion_x] == 2 || copia_mapa[i][posicion_x] == 6 || copia_mapa[i][posicion_x] == 7 || copia_mapa[i][posicion_x] == 8){
                            break;
                        }else if(i == jugador_y || i== totem_y ){
                            disparoEnemigo = true;
                            direccion = 4;
                            break;
                        }
                    }
                }
                
            }else if(posicion_y == jugador_y || posicion_y == totem_y){
                if(jugador_x - posicion_x < 0 ||totem_x - posicion_x < 0){
                    for (int i = posicion_x; i >= 0; i--){
                        if(copia_mapa[posicion_y][i] == 1 || copia_mapa[posicion_y][i] == 2|| copia_mapa[posicion_y][i] == 6 || copia_mapa[posicion_y][i] == 7 || copia_mapa[posicion_y][i] == 8){
                            break;
                        }else if(i == jugador_x || i== totem_x){
                            disparoEnemigo = true;
                            direccion = 2;
                            break;
                        }
                    }
                }else{
                    for (int i = posicion_x; i <= 9; i++){
                        if(copia_mapa[posicion_y][i] == 1 || copia_mapa[posicion_y][i] == 2|| copia_mapa[posicion_y][i] == 6 || copia_mapa[posicion_y][i] == 7 || copia_mapa[posicion_y][i] == 8){
                            break;
                        }else if(i == jugador_x || i== totem_x){
                            disparoEnemigo = true;
                            direccion = 1;
                            break;
                        }
                    }
                }
            }

            if(disparoEnemigo)
			{	
                int disparo_x = 0, disparo_y = 0;
                switch(direccion) {
                    case 1: // Este (derecha)
                        disparo_x++;
                        break;
                    case 2: // Oeste (izquierda)
                        disparo_x--;
                        break;
                    case 3: // Norte (arriba)
                        disparo_y--;
                        break;
                    case 4: // Sur (abajo)
                        disparo_y++;
                        break;
                }
                disparos.push_back({posicion_y, posicion_x, disparo_x, disparo_y});
                return;
				
			}
			
            int probabilidadDisparar = rand() % 10;
            if (probabilidadDisparar == 1){
                int disparo_x = 0, disparo_y = 0;
                switch(direccion) {
                    case 1: // Este (derecha)
                        for(int i = posicion_x; i< 12; i++)
                        	{
                        		if(copia_mapa[posicion_y][i] == 2)
                        		{
                        			return;
                        		}
                        	}
                        disparo_x++;
                        break;
                    case 2: // Oeste (izquierda)
                        
                        for(int i = posicion_x; i>=0 ; i--)
                    	{
                    		if(copia_mapa[posicion_y][i] == 2)
                    		{
                    			return;
                    		}
                    	}
                        disparo_x--;
                        break;
                    case 3: // Norte (arriba)
                    	for(int i = posicion_y; i>=0 ; i--)
                    	{
                    		if(copia_mapa[i][posicion_x] == 2)
                    		{
                    			return;
                    		}
                    	}
                        disparo_y--;
                        break;
                    case 4: // Sur (abajo)
                    	for(int i = posicion_y; i<=9 ; i++)
                    	{
                    		if(copia_mapa[i][posicion_x] == 2)
                    		{
                    			return;
                    		}
                    	}
                        disparo_y++;
                        break;
                }
                disparos.push_back({posicion_y, posicion_x, disparo_x, disparo_y, false, false});
            }
        }
        

};
#endif
