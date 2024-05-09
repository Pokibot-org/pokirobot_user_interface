#include "IHM_soundboard.h"

#include "./../pokraphic/pokraphic.h"
#include "./../pokraphic/PKC_button.h"
#include "./../audio/audio_system.h"

#include "fatfs.h"

char buttonRandomText[] = "Random";
PKC_buttonP buttonRandomParams;
PKC_item buttonRandom;
char buttonSoundboardBackText[] = "Retour";
PKC_buttonP buttonSoundboardBackParams;
PKC_item buttonSoundboardBack;


void (*IHM_soundboardCloseCallback)();

int IHM_soundboardIsOpen = 0;

int brendaSoundNbr = 0;


void getBrendaSoundNbr() {

    char path[256] = "brenda";

    FRESULT res;
    DIR dir;
    FILINFO fno;

    res = f_opendir(&dir, path); // Open the directory
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno); // Read a directory item
            if (res != FR_OK || fno.fname[0] == 0) break; // Error or end of dir
            if (fno.fattrib & AM_DIR) {
                // Directory
            } else {
                // File
                // printf("%10u %s\r\n", fno.fsize, fno.fname);
                brendaSoundNbr++;
            }
        }
        f_closedir(&dir);
    } else {
        printf("Failed to open \"%s\". (%u)\r\n", path, res);
    }
}

void getRandomSoundPath(char* path) {

    strcpy(path, "brenda");

    int index = rand()%brendaSoundNbr;

    FRESULT res;
    DIR dir;
    FILINFO fno;

    res = f_opendir(&dir, path); // Open the directory
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno); // Read a directory item
            if (res != FR_OK || fno.fname[0] == 0) break; // Error or end of dir
            if (fno.fattrib & AM_DIR) {
                // Directory
            } else {
                if (index == 0) {
                    strcpy(path, "brenda/");
                    strcpy(path+7, fno.fname);
                    return;
                }
                index--;
            }
        }
        f_closedir(&dir);
    } else {
        printf("Failed to open \"%s\". (%u)\r\n", path, res);
    }
}


void buttonSoundboardBackCallback(int event) {
    if (event&PKC_BUTTON_RELEASE) {
        IHM_soundboardCloseCallback();
    }
}
void buttonRandomCallback(int event) {
    char path[256];
    if (event&PKC_BUTTON_RELEASE) {
        getRandomSoundPath(path);
        ASYS_playFile(path, 100);
        //ASYS_repeatFile("fluf.wav", 0, 715000);
    }
}



void IHM_soundboardInit(void (*fun)()) {
    getBrendaSoundNbr();
    IHM_soundboardCloseCallback = fun;
    PKC_buttonInit(
        &buttonRandom, 10, 100, 200, 60, 10,
        &buttonRandomParams, (char*)buttonRandomText, &buttonRandomCallback
    );
    PKC_buttonInit(
        &buttonSoundboardBack, 10, 100 + 70, 200, 60, 10,
        &buttonSoundboardBackParams, (char*)buttonSoundboardBackText, &buttonSoundboardBackCallback
    );
}

void IHM_soundboardOpen() {
    IHM_soundboardIsOpen = 1;
    PKC_addItem(&buttonRandom);
    PKC_addItem(&buttonSoundboardBack);
}
void IHM_soundboardClose() {
    PKC_removeItem(&buttonRandom);
    PKC_removeItem(&buttonSoundboardBack);
    IHM_soundboardIsOpen = 0;
}
