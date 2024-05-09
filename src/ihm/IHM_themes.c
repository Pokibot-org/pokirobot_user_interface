#include "IHM_themes.h"

#include "fatfs.h"

#include "./../pokraphic/pokraphic.h"
#include "./../pokraphic/PKC_slider.h"
#include "./../pokraphic/PKC_combox.h"
#include "./../pokraphic/PKC_scrollList.h"
#include "./../pokraphic/PKC_button.h"
#include "./../pokraphic/PKC_poki.h"
#include "./../pokraphic/PKC_window.h"

#include "./../audio/audio_system.h"

//============================================================//
// Color objects
//============================================================//
char windowColorsText[] = "Couleurs";
PKC_windowP windowColorsParams;
PKC_item windowColors;

PKC_sliderP sliderRParams, sliderGParams, sliderBParams;
PKC_item sliderR, sliderG, sliderB;

PKC_scrollListP scrollListColorParams;
PKC_item scrollListColor;
char* scrollListColorItems[] = {
    "General", "Fond", "Sombre", "Moyen", "Clair"
};
PKC_item comboxColor;
PKC_comboxP comboxColorParams;

char buttonResetText[] = "R";
PKC_buttonP buttonResetParams;
PKC_item buttonReset;

char switchRGBText[] = "RGB";
PKC_buttonP switchRGBParams;
PKC_item switchRGB;


//============================================================//
// Sound objects
//============================================================//
char windowSoundText[] = "Son";
PKC_windowP windowSoundParams;
PKC_item windowSound;

PKC_item scrollListBlips;
PKC_scrollListP scrollListBlipsParams;
PKC_item comboxBlips;
PKC_comboxP comboxBlipsParams;
char* scrollListBlipsItems[] = {
    "Choix000", "Choix001", "Choix002", "Choix003", "Choix004",
    "Choix005", "Choix006", "Choix007", "Choix008", "Choix009",
    "Choix010", "Choix011", "Choix012", "Choix013", "Choix014",
    "Choix015", "Choix016", "Choix017", "Choix018", "Choix019",
    "Choix020", "Choix021", "Choix022", "Choix023", "Choix024",
    "Choix025", "Choix026", "Choix027", "Choix028", "Choix029",
    "Choix030", "Choix031", "Choix032", "Choix033", "Choix034",
    "Choix035", "Choix036", "Choix037", "Choix038", "Choix039",
    "Choix040", "Choix041", "Choix042", "Choix043", "Choix044",
    "Choix045", "Choix046", "Choix047", "Choix048", "Choix049"
};

PKC_item scrollListSout;
PKC_scrollListP scrollListSoutParams;
char* scrollListSoutItems[] = {
    "Principal", "Auxiliaire", "Aucun"
};


//============================================================//
// Poki objects
//============================================================//
char windowPokiText[] = "Poki";
PKC_windowP windowPokiParams;
PKC_item windowPoki;
PKC_sliderP sliderPokiParams;
PKC_item sliderPoki;

char switchPokiSoundText[] = "Son bords";
PKC_buttonP switchPokiSoundParams;
PKC_item switchPokiSound;
char switchPokiBounceText[] = "Rebonds";
PKC_buttonP switchPokiBounceParams;
PKC_item switchPokiBounce;

PKC_item scrollListPokipic;
PKC_scrollListP scrollListPokipicParams;
char* scrollListPokipicItems[] = {
    "Poki origin", "Poki 2024", "Poki univers"
};


// TODO:
// - Add picture selection
// - Add color mode


//============================================================//
// Common objects
//============================================================//
char buttonBackText[] = "Retour";
PKC_buttonP buttonBackParams;
PKC_item buttonBack;

char buttonSaveText[] = "Enreg.";
PKC_buttonP buttonSaveParams;
PKC_item buttonSave;

//============================================================//
// Variables
//============================================================//
extern PKC_item poki;

int red, green, blue;
int comboxIndex = 0;

// Global close callback for parameters IHM
void (*closeCallback)();

//============================================================//
// Prototypes
//============================================================//
void sliderPokiCallback(int value);

//============================================================//
// Private functions
//============================================================//
void writeConfToSD() {
    FIL fil;
    if (disk_status(0) == FR_OK && f_open(&fil, "CFTH.TXT", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {
        f_printf(&fil, "%d\n", PKC_theme.color0);
        f_printf(&fil, "%d\n", PKC_theme.color1);
        f_printf(&fil, "%d\n", PKC_theme.color2);
        f_printf(&fil, "%d\n", PKC_theme.backGroundColor);
        f_printf(&fil, "%s\n", PKC_theme.blipName);
        f_printf(&fil, "%d\n", scrollListSoutParams.index);
        f_printf(&fil, "%d\n", PKC_sliderGetValue(&sliderPoki));
        f_printf(&fil, "%d\n", switchPokiSoundParams.value);
        f_printf(&fil, "%d\n", switchPokiBounceParams.value);
        f_printf(&fil, "%d\n", poki.x);
        f_printf(&fil, "%d\n", poki.y);
        f_close(&fil);
        printf("Written\r\n");
    } else {
        printf("Write error\r\n");
    }
}

void readConfFromSD() {
    FIL fil;
    char buffer[256];
    int value;
    if (disk_status(0) == FR_OK && f_open(&fil, "CFTH.TXT", FA_READ) == FR_OK) {
        f_gets((void*)buffer, 32, &fil);
        sscanf(buffer, "%d", (int*)&(PKC_theme.color0));
        f_gets((void*)buffer, 32, &fil);
        sscanf(buffer, "%d", (int*)&(PKC_theme.color1));
        f_gets((void*)buffer, 32, &fil);
        sscanf(buffer, "%d", (int*)&(PKC_theme.color2));
        f_gets((void*)buffer, 32, &fil);
        sscanf(buffer, "%d", (int*)&(PKC_theme.backGroundColor));

        f_gets((void*)buffer, 32, &fil);
        sscanf(buffer, "%s", PKC_theme.blipName);
        f_gets((void*)buffer, 32, &fil);
        sscanf(buffer, "%d", (int*)&(value));
        PKC_scrollListSetIndex(&scrollListSout, value);

        f_gets((void*)buffer, 32, &fil);
        sscanf(buffer, "%d", (int*)&(value));
        f_gets((void*)buffer, 32, &fil);
        PKC_sliderSetValue(&sliderPoki, value);
        sliderPokiCallback(value);
        sscanf(buffer, "%d", (int*)&(value));
        PKC_switchSetValue(&switchPokiSound, value);
        f_gets((void*)buffer, 32, &fil);
        sscanf(buffer, "%d", (int*)&(value));
        PKC_switchSetValue(&switchPokiBounce, value);
        f_gets((void*)buffer, 32, &fil);
        sscanf(buffer, "%d", (int*)&(poki.x));
        f_gets((void*)buffer, 32, &fil);
        sscanf(buffer, "%d", (int*)&(poki.y));

        f_close(&fil);
    } else {
        // File does not exist
        // Create file with default parameters
        writeConfToSD();
    }
}


void callbackRGB();

inline void callbackRGB() {
    uint32_t color = 0xFF000000 | (red<<16) | (green<<8) | (blue<<0);
    switch (comboxIndex) {
        case 0:
            PKC_setColorTheme(color);
            break;
        case 1:
            PKC_theme.backGroundColor = color;
            break;
        case 2:
            PKC_theme.color2 = color;
            break;
        case 3:
            PKC_theme.color0 = color;
            break;
        case 4:
            PKC_theme.color1 = color;
            break;
        default:
            break;
    }
}
void callbackRed(int value) {
    red = value/4;
    callbackRGB();
}
void callbackGreen(int value) {
    green = value/4;
    callbackRGB();
}
void callbackBlue(int value) {
    blue = value/4;
    callbackRGB();
}

void scrollListColorCallback(int index) {
    PKC_comboxScrollListCallback(&comboxColor, index);
}
void comboxColorCallback(int index) {
    comboxIndex = index;
    switch (comboxIndex) {
        case 0:
            red = (PKC_theme.color1>>16) & 0xFF;
            green = (PKC_theme.color1>>8) & 0xFF;
            blue = (PKC_theme.color1>>0) & 0xFF;
            break;
        case 1:
            red = (PKC_theme.backGroundColor>>16) & 0xFF;
            green = (PKC_theme.backGroundColor>>8) & 0xFF;
            blue = (PKC_theme.backGroundColor>>0) & 0xFF;
            break;
        case 2:
            red = (PKC_theme.color2>>16) & 0xFF;
            green = (PKC_theme.color2>>8) & 0xFF;
            blue = (PKC_theme.color2>>0) & 0xFF;
            break;
        case 3:
            red = (PKC_theme.color0>>16) & 0xFF;
            green = (PKC_theme.color0>>8) & 0xFF;
            blue = (PKC_theme.color0>>0) & 0xFF;
            break;
        case 4:
            red = (PKC_theme.color1>>16) & 0xFF;
            green = (PKC_theme.color1>>8) & 0xFF;
            blue = (PKC_theme.color1>>0) & 0xFF;
            break;
        default:
            break;
    }
    PKC_sliderWriteValue(&sliderR, (red)*4);
    PKC_sliderWriteValue(&sliderG, (green)*4);
    PKC_sliderWriteValue(&sliderB, (blue)*4);
}

void buttonResetCallback(int event) {
    if (event&PKC_BUTTON_RELEASE) {
        PKC_setColorTheme(PKC_DEFAULT_MAIN_COLOR);
        PKC_theme.backGroundColor = PKC_DEFAULT_BACKGROUND_COLOR;
        comboxColorCallback(comboxIndex);
    }
}

void switchRGBCallback(int event) {
    if (event) {
        PKC_setRGBMode(1);
    } else {
        PKC_setRGBMode(0);
    }
}


void initBlipsScrollText(PKC_scrollListP* scrollListParams) {
    FRESULT res;
    DIR dir;
    FILINFO fno;
    int nfile;
    if (disk_status(0) == FR_OK) {
        // Up to 50 files, else who knows what happen ? (a kraken could appear behind your chair)
        char* buffer = malloc(50*9*sizeof(char));
        res = f_opendir(&dir, "blips");
        if (res == FR_OK) {
            nfile = 0;
            for (;;) {
                res = f_readdir(&dir, &fno);
                if (res != FR_OK || fno.fname[0] == 0) break;
                if (fno.fattrib & AM_DIR) {
                } else {
                    strcpy(buffer, fno.fname);
                    scrollListParams->charArray[nfile] = buffer;
                    buffer = buffer + strlen(fno.fname) + 1;
                    nfile++;
                }
            }
            scrollListParams->itemCount = nfile;
            f_closedir(&dir);
        }
        // Did you check there was no kraken behind your chair ?
    }
}

// Sound object functions
void comboxBlipsCallback(int index) {
    strcpy(PKC_theme.blipName+6, scrollListBlipsParams.charArray[index]);
}
void scrollListBlipsCallback(int index) {
    PKC_comboxScrollListCallback(&comboxBlips, index);
}
void scrollListSoutCallback(int index) {
    if (index == 0) {
        ASYS_setOutputMode(ASYS_OUT_MAIN);
    } else if (index == 1) {
        ASYS_setOutputMode(ASYS_OUT_AUX);
    } else if (index == 2) {
        ASYS_setOutputMode(ASYS_OUT_NONE);
    }
}

// Poki functions
void sliderPokiCallback(int value) {
    ((PKC_pokiP*)(poki.params))->newSize = (400-16)*value/1024 + 16;
}
void switchPokiSoundCallback(int event) {
}
void switchPokiBounceCallback(int event) {
    if (event) {
        ((PKC_pokiP*)(poki.params))->bounce = 1;
    } else {
        ((PKC_pokiP*)(poki.params))->bounce = 0;
    }
}
void scrollListPokipicCallback(int index) {
    ((PKC_pokiP*)(poki.params))->picIndex = index;
    ((PKC_pokiP*)(poki.params))->picUpdate = 1;
}


// Common objects functions
void buttonBackCallback(int event) {
    if (event&PKC_BUTTON_RELEASE) {
        closeCallback();
    }
}
void buttonSaveCallback(int event) {
    if (event&PKC_BUTTON_RELEASE) {
        writeConfToSD();
    }
}

//============================================================//
// Public functions
//============================================================//
void IHM_themesInit(void (*fun)()) {
    closeCallback = fun;

    // Colors
    PKC_windowInit(
        &windowColors, 600, 0, 200, 480, 9,
        &windowColorsParams, (char*)windowColorsText
    );
    PKC_scrollListInit(
        &scrollListColor, windowColors.x + 10, windowColors.y+50+10+45, windowColors.width - 20, 160, 11,
        &scrollListColorParams, 5, scrollListColorItems, &scrollListColorCallback
    );
    PKC_comboxInit(
        &comboxColor, windowColors.x + 10, windowColors.y+50+10, windowColors.width - 20, 50, 10,
        &comboxColorParams, &scrollListColor, &comboxColorCallback
    );
    PKC_buttonInit(
        &buttonReset, windowColors.x + 10, windowColors.y+windowColors.height-10-60, 60, 60, 10,
        &buttonResetParams, (char*)buttonResetText, &buttonResetCallback
    );
    PKC_switchInit(
        &switchRGB, windowColors.x+10+buttonReset.width+5, windowColors.y+windowColors.height-10-60, windowColors.width-10-(10+buttonReset.width+5), 60, 10,
        &switchRGBParams, (char*)switchRGBText, &switchRGBCallback
    );
    int sldY = windowColors.y + 5 + 50 + 5 + 50 + 5;
    int sldW = (windowColors.width-5) / 3;
    int sldH = (windowColors.height-50-60-60-20);
    int sldX = windowColors.x + 10;
    PKC_sliderInit(
        &sliderR, sldX + 0*sldW, sldY, 50, sldH, 10,
        &sliderRParams, &callbackRed
    );
    PKC_sliderInit(
        &sliderG, sldX + 1*sldW, sldY, 50, sldH, 10,
        &sliderGParams, &callbackGreen
    );
    PKC_sliderInit(
        &sliderB, sldX + 2*sldW, sldY, 50, sldH, 10,
        &sliderBParams, &callbackBlue
    );
    sliderR.theme = &PKC_themeRed;
    sliderG.theme = &PKC_themeGreen;
    sliderB.theme = &PKC_themeBlue;

    // Sound objects
    PKC_windowInit(
        &windowSound, 300, 0, 305, 480, 9,
        &windowSoundParams, (char*)windowSoundText
    );
    PKC_scrollListInit(
        &scrollListBlips, windowSound.x + 10, windowSound.y+50+10+45, windowSound.width - 20 - 55, 200, 11,
        &scrollListBlipsParams, 50, scrollListBlipsItems, &scrollListBlipsCallback
    );
    initBlipsScrollText(&scrollListBlipsParams);
    PKC_comboxInit(
        &comboxBlips, windowSound.x + 10, windowSound.y+50+10, windowSound.width - 20 - 55, 50, 10,
        &comboxBlipsParams, &scrollListBlips, &comboxBlipsCallback
    );
    PKC_scrollListInit(
        &scrollListSout, comboxBlips.x, comboxBlips.y+comboxBlips.height+5, comboxBlips.width, 116, 10,
        &scrollListSoutParams, 3, scrollListSoutItems, &scrollListSoutCallback
    );

    // Poki objects
    PKC_windowInit(
        &windowPoki, 0, 0, 305, 415, 9,
        &windowPokiParams, (char*)windowPokiText
    );
    PKC_sliderInit(
        &sliderPoki, windowPoki.x+windowPoki.width-50-5-5, windowPoki.y+50+10, 50, windowPoki.height-50-10-10, 10,
        &sliderPokiParams, &sliderPokiCallback
    );
    PKC_switchInit(
        &switchPokiSound, windowPoki.x+10, windowPoki.y+50+10, windowPoki.width-10-50-15, 60, 10,
        &switchPokiSoundParams, (char*)switchPokiSoundText, &switchPokiSoundCallback
    );
    PKC_switchInit(
        &switchPokiBounce, switchPokiSound.x, switchPokiSound.y+switchPokiSound.height+5, switchPokiSound.width, switchPokiSound.height, 10,
        &switchPokiBounceParams, (char*)switchPokiBounceText, &switchPokiBounceCallback
    );
    PKC_scrollListInit(
        &scrollListPokipic, switchPokiBounce.x, switchPokiBounce.y+switchPokiBounce.height+5, switchPokiBounce.width, 116, 10,
        &scrollListPokipicParams, 3, scrollListPokipicItems, &scrollListPokipicCallback
    );

    // Global objects
    PKC_buttonInit(
        &buttonSave, 0, 420, 145, 60, 10,
        &buttonSaveParams, (char*)buttonSaveText, &buttonSaveCallback
    );
    PKC_buttonInit(
        &buttonBack, 150, 420, 145, 60, 10,
        &buttonBackParams, (char*)buttonBackText, &buttonBackCallback
    );

    // Setup objects
    readConfFromSD();

    comboxColorCallback(0);
    int i = 0;
    while (i < scrollListBlipsParams.itemCount && strcmp(PKC_theme.blipName+6, scrollListBlipsParams.charArray[i]) != 0) {
        i++;
    }
    if (i < scrollListBlipsParams.itemCount) {
        scrollListBlipsParams.index = i;
        PKC_comboxScrollListCallback(&comboxBlips, i);
    } else {
        scrollListBlipsParams.index = 0;
        PKC_comboxScrollListCallback(&comboxBlips, 0);
    }
}

void IHM_themesOpen() {
    PKC_addItem(&windowColors);
    PKC_addItem(&scrollListColor);
    PKC_addItem(&comboxColor);
    PKC_addItem(&sliderR);
    PKC_addItem(&sliderG);
    PKC_addItem(&sliderB);
    PKC_addItem(&buttonReset);
    PKC_addItem(&switchRGB);

    PKC_addItem(&windowSound);
    PKC_addItem(&scrollListBlips);
    PKC_addItem(&comboxBlips);
    PKC_addItem(&scrollListSout);

    PKC_addItem(&windowPoki);
    PKC_addItem(&sliderPoki);
    PKC_addItem(&switchPokiSound);
    PKC_addItem(&switchPokiBounce);
    PKC_addItem(&scrollListPokipic);

    PKC_addItem(&buttonSave);
    PKC_addItem(&buttonBack);
}

void IHM_themesClose() {
    PKC_removeItem(&windowColors);
    PKC_removeItem(&scrollListColor);
    PKC_removeItem(&comboxColor);
    PKC_removeItem(&sliderR);
    PKC_removeItem(&sliderG);
    PKC_removeItem(&sliderB);
    PKC_removeItem(&buttonReset);
    PKC_removeItem(&switchRGB);

    PKC_removeItem(&windowSound);
    PKC_removeItem(&scrollListBlips);
    PKC_removeItem(&comboxBlips);
    PKC_removeItem(&scrollListSout);

    PKC_removeItem(&windowPoki);
    PKC_removeItem(&sliderPoki);
    PKC_removeItem(&switchPokiSound);
    PKC_removeItem(&switchPokiBounce);
    PKC_removeItem(&scrollListPokipic);

    PKC_removeItem(&buttonSave);
    PKC_removeItem(&buttonBack);
}