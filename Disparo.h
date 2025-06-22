#ifndef DISPARO_H
#define DISPARO_H

#include <vector>

//------------------------------------------------------------------------------
// Estructura Disparo:
// Representa un proyectil en vuelo dentro del juego.
// y, x: coordenadas actuales en la cuadrícula del mapa.
// dx, dy: velocidad/dirección de desplazamiento en ejes y y x.
// fromPlayer: verdadero si el disparo fue iniciado por el jugador.
// fromBoss: verdadero si el disparo fue iniciado por un jefe
//------------------------------------------------------------------------------
struct Disparo {
    int y;             // Posición vertical (fila)
    int x;             // Posición horizontal (columna)
    int dx;            // Desplazamiento horizontal por paso
    int dy;            // Desplazamiento vertical por paso
    bool fromPlayer;   // Indica si proviene del jugador
    bool fromBoss;     // Indica si proviene de un jefe
};

// Vector global que almacena todos los disparos en vuelo
extern std::vector<Disparo> disparos;

#endif 
