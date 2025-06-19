#ifndef Enemigo_nivel_dos_h
#define Enemigo_nivel_dos_h

#include "enemigo.h"



class enemigo_nivel_dos : public Enemigo
{
public:
	
	enemigo_nivel_dos(int x, int y) : Enemigo(x, y)
	{	
	this->velocidad_disparo = 14;
	sprite = {
            {L" ┷ ", L"┫█┣", L" ┯ "}, // derecha
            {L" ┷ ", L"┫█┣", L" ┯ "},   // izquierda	
            {L" ┷ ", L"┫█┣", L" ┯"},   // arriba
            {L" ┷ ", L"┫█┣", L" ┯"},  // abajo
        };
       	
	}
	
	void disparar(WINDOW* ventana) override 
	{
	 bool disparoEnemigo = false;

            if (contador_disparo!=velocidad_disparo || !vivo){
                contador_disparo++;
                return;
            }
            contador_disparo = 0;

                disparos.push_back({posicion_y, posicion_x, 1, 0, false, false});
                disparos.push_back({posicion_y, posicion_x, -1, 0, false, false});
                disparos.push_back({posicion_y, posicion_x, 0, 1, false, false});
                disparos.push_back({posicion_y, posicion_x, 0, -1, false, false});
            
        }

};
#endif

