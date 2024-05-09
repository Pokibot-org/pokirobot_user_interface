#ifndef SWH_SWITCHS_H
#define SWH_SWITCHS_H


enum SWH_tiretteState {
    SWH_tiretteNotPlaced,
    SWH_tirettePlaced,
    SWH_tiretteRemoved,
};

void SWH_init();
void SWH_tick();

#endif