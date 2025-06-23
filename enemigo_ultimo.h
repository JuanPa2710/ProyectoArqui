#ifndef Enemigo_ultimo_h
#define Enemigo_ultimo_h

#include "enemigo.h"

class enemigo_ultimo : public Enemigo 
{
private:
	int impactos_destruccion = 3; //Atibuto que indica cuantos disparos soporta el jefe antes de ser destruido
public:
	enemigo_ultimo(int x, int y): Enemigo (x,y)
	{

        	sprite = {
                {L"╔═╗", L"██►", L"╚═╝"}, // derecha
                {L"╔═╗", L"◄██", L"╚═╝"},   // izquierda	
                {L" ▲", L"║█║", L" ━"},   // arriba
                {L" ━ ", L"║█║", L" ▼"},  // abajo
            };
       
       this->velocidad_disparo =1;
       this->velocidad_enemigo = 7;
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
            if(posicion_x == jugador_x){
                if(jugador_y - posicion_y < 0){
                    for (int i = posicion_y; i >= jugador_y; i--){
                        if(copia_mapa[i][posicion_x] == 1){
                            break;
                        }else if(i == jugador_y || copia_mapa[i][posicion_x] == 2 ){
                            disparoEnemigo = true;
                            direccion = 3;
                            break;
                        }
                    }
                }else{
                    for (int i = posicion_y; i <= jugador_y; i++){
                        if(copia_mapa[i][posicion_x] == 1 ){
                            break;
                        }else if(i == jugador_y || copia_mapa[i][posicion_x] == 2){
                            disparoEnemigo = true;
                            direccion = 4;
                            break;
                        }
                    }
                }
                
            }else if(posicion_y == jugador_y){
                if(jugador_x - posicion_x < 0){
                    for (int i = posicion_x; i >= jugador_x; i--){
                        if(copia_mapa[posicion_y][i] == 1){
                            break;
                        }else if(i == jugador_x || copia_mapa[posicion_y][i] == 2){
                            disparoEnemigo = true;
                            direccion = 2;
                            break;
                        }
                    }
                }else{
                    for (int i = posicion_x; i <= jugador_x; i++){
                        if(copia_mapa[posicion_y][i] == 1 ){
                            break;
                        }else if(i == jugador_x || copia_mapa[posicion_y][i] == 2){
                            disparoEnemigo = true;
                            direccion = 1;
                            break;
                        }
                    }
                }
            }


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
                for(int i = posicion_x; 12>i; i++){
                    
                }
                disparos.push_back({posicion_y, posicion_x, disparo_x, disparo_y, false, true});

            
        }

	/*Retorna los impactos que le quedan al jefe para ser destruido*/
        int get_impactos_destruccion() const override
        {
        	return impactos_destruccion;
        
        }

	/*Reduce la cantidad de impactos necesaria para destruir al jefe*/
        void nuevo_impacto() override
        {
        	impactos_destruccion--;
        }
		
};

#endif
