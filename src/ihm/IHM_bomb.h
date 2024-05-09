#ifndef IHM_BOMB_H
#define IHM_BOMB_H

void IHM_bombInit(void (*fun)());
void IHM_bombOpen(int value);
void IHM_bombClose();
void IHM_bombTick();

#endif