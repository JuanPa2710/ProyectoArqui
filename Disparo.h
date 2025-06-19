#ifndef DISPARO_H
#define DISPARO_H

#include <vector>

struct Disparo {
    int y, x;
    int dx, dy;
    bool fromPlayer;
    bool fromBoss;
};

extern std::vector<Disparo> disparos;

#endif 
