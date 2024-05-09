#include "IHM_match.h"

#include "./../pokraphic/pokraphic.h"
#include "./../pokraphic/PKC_button.h"
#include "./../pokraphic/PKC_segments.h"
#include "./../pokraphic/PKC_window.h"
#include "./../pokraphic/PKC_numPad.h"
#include "./../pokraphic/PKC_log.h"
#include "./../pokraphic/PKC_smallBatt.h"

#include "./../led/LED_led.h"



#define MATCH_DURATION 100


char windowScoreText[] = "Score";
PKC_buttonP windowScoreParams;
PKC_item windowScore;

PKC_item segmentsMatchScore;
PKC_segmentsP segmentsMatchScoreParams;


char windowControlText[] = "Control";
PKC_buttonP windowControlParams;
PKC_item windowControl;

uint32_t matchColors[3] = {0xFFFFFFFF, 0xFF005CE6, 0xFFE6AC00};
int IHM_matchSelectedColor = 0;
char buttonMatchBackText[] = "Retour";
PKC_buttonP buttonMatchBackParams;
PKC_item buttonMatchBack;
PKC_item smallBattMatch;
PKC_smallBattP smallBattMatchParams;
PKC_item segmentsMatchTime;
PKC_segmentsP segmentsMatchTimeParams;



PKC_item logMatch;
PKC_logP logMatchParams;



// Numpad is common with main app
extern PKC_item numPad;
extern PKC_numPadP numPadParams;

void (*IHM_matchCloseCallback)();

int IHM_matchIsOpen = 0;

uint32_t IHM_matchStartTick;
int matchStarted = 0;
int matchStopped = 0;

void IHM_matchTiretteReleased() {
    IHM_matchStartTick = HAL_GetTick();
    matchStarted = 1;
    PKC_logAddLine(&logMatch, 0xFF00FF00, "|I| Debut du match");
}

void IHM_matchTirettePlugged() {
    PKC_logAddLine(&logMatch, 0xFF00FF00, "|I| Tirette inseree (rip les accents)");
}

void IHM_matchSetColor(int c) {
    if (c >= 0 && c < 3) {
        IHM_matchSelectedColor = c;
        PKC_setColorTheme(matchColors[IHM_matchSelectedColor]);
        if (IHM_matchSelectedColor == 1) {
            LED_setColor(LED_TOP_LEFT, 0x00, 0x00, 0x00);
            LED_setColor(LED_BOTTOM_LEFT, 0x00, 0x5C, 0xE6);
        } else if (IHM_matchSelectedColor == 2) {
            LED_setColor(LED_TOP_LEFT, 0xE6, 0xAC, 0x00);
            LED_setColor(LED_BOTTOM_LEFT, 0x00, 0x00, 0x00);
        } else {
            LED_setColor(LED_TOP_LEFT, 0x00, 0x00, 0x00);
            LED_setColor(LED_BOTTOM_LEFT, 0x00, 0x00, 0x00);
        }
    }
}

void numPadMatchCallback(int mode, float value) {
    if (mode) {
        PKC_segmentsSetTarget(&segmentsMatchScore, value);
    }
}

void IHM_segmentsMatchTsEvent(PKC_item* item, FT6336G_point* tsPoint) {
    if ((tsPoint->event & TS_CONTACT) && tsPoint->longPress) {
        PKC_numPadOpen(&numPad, 0, "Valeur du score", &numPadMatchCallback);
    }
}

//IHM_matchTiretteReleased(); // For test purpose

void buttonMatchBackCallback(int event) {
    if (event&PKC_BUTTON_RELEASE) {
        IHM_matchCloseCallback();
    }
}

void IHM_matchInit(void (*fun)()) {
    IHM_matchCloseCallback = fun;

    PKC_windowInit(
        &windowScore, 0, 0, 600, 360, 9,
        &windowScoreParams, (char*)windowScoreText
    );
    PKC_segmentsInit(
        &segmentsMatchScore, windowScore.x + 10, windowScore.y + 60, windowScore.width - 20, windowScore.height - 70, 10,
        &segmentsMatchScoreParams, 3
    );
    segmentsMatchScore.tickEnable = 0;

    PKC_windowInit(
        &windowControl, windowScore.x+windowScore.width-5, 0, 800-windowScore.width+5, windowScore.height, 9,
        &windowControlParams, (char*)windowControlText
    );
    PKC_buttonInit(
        &buttonMatchBack, windowControl.x+10, windowControl.y+60+60+5, windowControl.width-10-10, 60, 10,
        &buttonMatchBackParams, (char*)buttonMatchBackText, &buttonMatchBackCallback
    );
    PKC_smallBattInit(
        &smallBattMatch, buttonMatchBack.x, buttonMatchBack.y+60+5, buttonMatchBack.width, 60, 10,
        &smallBattMatchParams
    );
    PKC_segmentsInit(
        &segmentsMatchTime, windowControl.x + 10, windowControl.y+windowControl.height-100-10, windowControl.width-10-10, 100, 10,
        &segmentsMatchTimeParams, 3
    );
    PKC_segmentsSetValue(&segmentsMatchTime, MATCH_DURATION);
    segmentsMatchTime.tickEnable = 0;

    PKC_logInit(
        &logMatch, 0, windowScore.y+windowScore.height-5, 800, 480-windowScore.height+5, 9,
        &logMatchParams
    );
    // PKC_logAddLine(&logMatch, 0xFFFF0000, "/!\\ Tirette non detectee");
    // PKC_logAddLine(&logMatch, 0xFFFFFF00, "(!) Batterie faible");
    // PKC_logAddLine(&logMatch, 0xFF0000FF, "|I| Arret d'urgence relache");
    PKC_logAddLine(&logMatch, 0xFF000000, "    Info inutile");
    PKC_logAddLine(&logMatch, 0xFF000000, "    Autre info inutile");
}

void IHM_matchOpen() {
    PKC_addItem(&windowScore);
    segmentsMatchScore.tsEvent = IHM_segmentsMatchTsEvent;
    PKC_addItem(&segmentsMatchScore);
    segmentsMatchScore.tickEnable = 1;

    PKC_addItem(&windowControl);
    PKC_addItem(&buttonMatchBack);
    PKC_addItem(&smallBattMatch);
    PKC_addItem(&segmentsMatchTime);

    PKC_addItem(&logMatch);

    IHM_matchIsOpen = 1;
}
void IHM_matchClose() {
    PKC_removeItem(&windowScore);
    segmentsMatchScore.tsEvent = 0;
    PKC_removeItem(&segmentsMatchScore);
    segmentsMatchScore.tickEnable = 0;

    PKC_removeItem(&windowControl);
    PKC_removeItem(&buttonMatchBack);
    PKC_removeItem(&smallBattMatch);
    PKC_removeItem(&segmentsMatchTime);

    PKC_removeItem(&logMatch);

    IHM_matchIsOpen = 0;
}

void IHM_matchTick() {
    if (IHM_matchIsOpen) {
        if (matchStarted && !matchStopped) {
            int remainingSeconds = MATCH_DURATION-(HAL_GetTick()-IHM_matchStartTick)/1000;
            if (remainingSeconds <= 0) {
                PKC_segmentsSetValue(&segmentsMatchTime, 0);
                PKC_removeItem(&segmentsMatchTime);
                PKC_logAddLine(&logMatch, 0xFF0000FF, "|I| Fin du match");
                matchStopped = 1;
            } else {
                PKC_segmentsSetValue(&segmentsMatchTime, remainingSeconds);
            }
        }
    }
}