#ifndef IHM_MATCH_H
#define IHM_MATCH_H


void IHM_matchTiretteReleased();
void IHM_matchTirettePlugged();

void IHM_matchInit(void (*fun)());
void IHM_matchOpen();
void IHM_matchClose();
void IHM_matchTick();

void IHM_matchSetColor(int c);

#endif