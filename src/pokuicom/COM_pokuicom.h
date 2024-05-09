#ifndef COM_POKUICOM_H
#define COM_POKUICOM_H

#include "poktocol.h"
#include "stdint.h"


int PCOM_init(void);

void PCOM_send_team_color(enum pokprotocol_team color);
void PCOM_notify_start_of_match();

void PCOM_send_score(uint8_t score);

#endif