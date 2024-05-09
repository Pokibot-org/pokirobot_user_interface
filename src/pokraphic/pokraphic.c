#include "pokraphic.h"

#include "stm32f4xx_hal.h"

#include "./../audio/audio_system.h"

#include "fatfs.h"

extern LTDC_HandleTypeDef hltdc_eval;
static DMA2D_HandleTypeDef           hdma2d;
extern DSI_HandleTypeDef hdsi_eval;

#define VSYNC    1
#define VBP      1
#define VFP      1
#define VACT     480
#define HSYNC    1
#define HBP      1
#define HFP      1
#define HACT     800

#define PKC_MAX_REFRESH_ZONES 32
PKC_refreshZone PKC_refreshZones[PKC_MAX_REFRESH_ZONES];
int PKC_firstRZ = 0;
int PKC_lastRZ = 0;
int PKC_refreshAll = 0;

uint32_t PKC_soundFlags = 0;

static const uint32_t Buffers[] = {
    LAYER0_ADDRESS,
    LAYER0_ADDRESS + LCD_BUFFER_SIZE,
};

static __IO int32_t front_buffer   = 0;
static __IO int32_t pend_buffer   = 1;
static __IO int32_t dsiInterruptDone = 1;

uint8_t lcd_status = LCD_OK;

static uint8_t LCD_Init(void);
void LTDC_Init(void);


#define MAX_ITEM_COUNT 32
PKC_item* itemArray[MAX_ITEM_COUNT];
int itemCount = 0;

int rgbState = 0;

// FPS statistic
int PKC_FPS;

static uint32_t tickTimer1;

// It took me a long time to believe in this code, but it actually works
// (yes, in ALL cases (if width and height are positives of course))
int PKC_isItemInRefreshZone(const PKC_item* item, const PKC_refreshZone* refreshZone) {
    return item->x+item->width  > refreshZone->x && item->x < refreshZone->x+refreshZone->width &&
           item->y+item->height > refreshZone->y && item->y < refreshZone->y+refreshZone->height;
}
int PKC_isItemInItem(const PKC_item* item, const PKC_item* item2) {
    return item->x+item->width  > item2->x && item->x < item2->x+item2->width &&
           item->y+item->height > item2->y && item->y < item2->y+item2->height;
}

int PKC_checkRefreshZones(PKC_item* item) {
    for (int i = PKC_firstRZ; i != PKC_lastRZ; i = (i+1)%PKC_MAX_REFRESH_ZONES) {
        if (PKC_isItemInRefreshZone(item, PKC_refreshZones + i)) {
            // item->needRefresh += 1;
            PKC_setRefreshItemFlag(item);
            return 1;
        }
    }
    return 0;
}

void PKC_cleanRefreshZones() {
    for (int i = PKC_firstRZ; i != PKC_lastRZ; i = (i+1)%PKC_MAX_REFRESH_ZONES) {
        PKC_refreshZones[i].timeToLive--;
        if (PKC_refreshZones[i].timeToLive <= 0) {
            PKC_firstRZ = (PKC_firstRZ+1)%PKC_MAX_REFRESH_ZONES;
        }
    }
}

void PKC_setRefreshFlagItemsUpperLevelel(PKC_item* item);
void PKC_setRefreshFlagItemsUpperLevelel(PKC_item* item) {
    item->needRefresh = 1;
    for (int i = 0; i < itemCount; i++) {
        if (itemArray[i] != item && itemArray[i]->drawEnable && itemArray[i]->needRefresh == 0 &&
        itemArray[i]->level > item->level && PKC_isItemInItem(item, itemArray[i])) {
            PKC_setRefreshFlagItemsUpperLevelel(itemArray[i]);
        }
    }
}

void PKC_setColorTheme(uint32_t color) {
    int r = (color>>16) & 0xFF;
    int g = (color>>8)  & 0xFF;
    int b = (color>>0)  & 0xFF;
    PKC_theme.color1 = 0xFF000000 | color;
    PKC_theme.color0 = 0xFF000000 | ((2*r/3)<<16) | ((2*g/3)<<8) | (2*b/3);
    PKC_theme.color2 = 0xFF000000 | ((r/3)<<16) | ((g/3)<<8) | (b/3);

    PKC_addRefreshZoneAll();
}

void PKC_rollColorTheme() {
    rgbState = (rgbState + 1) & 0xFF;
    PKC_setColorTheme(BSP_LCD_getRainbowColor(rgbState));
}

void PKC_addItem(PKC_item* item) {
    if (itemCount + 1 < MAX_ITEM_COUNT) {
        int i = itemCount;
        while (i > 0 && itemArray[i-1]->level > item->level) {
            itemArray[i] = itemArray[i-1];
            i--;
        }
        itemArray[i] = item;
        itemCount++;
        // item->needRefresh = 2;
        PKC_setRefreshItemFlag(item);
    }
}
inline void PKC_removeItem(PKC_item* item) {
    int i = 0;
    while (i < itemCount && itemArray[i] != item) {
        i++;
    }
    if (i < itemCount) {
        PKC_addRefreshZoneFromItem(item);
        itemCount--;
        while (i < itemCount) {
            itemArray[i] = itemArray[i+1];
            i++;
        }
    }
}

int PKC_init() {
    /* Initialize the LCD   */
    pend_buffer = 0;
    lcd_status = LCD_Init();
    if (lcd_status != LCD_OK) {
        return 1;
    }
    BSP_LCD_LayerDefaultInit(0, LAYER0_ADDRESS);
    BSP_LCD_SelectLayer(0);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_FillRect(0, 0, 800, 480);
    PKC_CopyBuffer((uint32_t *)Buffers[0], (uint32_t *)Buffers[1], 0, 0, 800, 480);
    HAL_Delay(5);

    dsiInterruptDone = 0;
    /*Refresh the LCD display*/
    HAL_DSI_Refresh(&hdsi_eval);

    // PKC_setColorTheme(BSP_LCD_getRainbowColor(0x6C));
    PKC_setColorTheme(PKC_DEFAULT_MAIN_COLOR);
    PKC_theme.backGroundColor = PKC_DEFAULT_BACKGROUND_COLOR;

    tickTimer1 = HAL_GetTick();

    return 0;
}


int rgbMode = 0;
int rgbCode = 0;

void PKC_tick() {

    if (rgbMode) {
        rgbCode = (rgbCode+1)&0xFF;
        uint32_t argb_color = BSP_LCD_getRainbowColor(rgbCode);
        PKC_setColorTheme(argb_color);
    }
    for (int i = 0; i < itemCount; i++) {
        if (itemArray[i]->tickEnable) {
            itemArray[i]->tick(itemArray[i]);
        }
    }
    if (PKC_soundFlags & PKC_SND_SPAWN) {
        PKC_soundFlags = 0;
        ASYS_playFile("spaw.wav", 110);
    } else if (PKC_soundFlags & PKC_SND_POKI) {
        PKC_soundFlags = 0;
        ASYS_playFile("pok2.wav", 110);
    } else if (PKC_soundFlags & PKC_SND_BLIP00) {
        PKC_soundFlags = 0;
        ASYS_playFile(PKC_theme.blipName, 90);
    } else if (PKC_soundFlags & PKC_SND_SONIC) {
        PKC_soundFlags = 0;
        ASYS_playFile("sound/sonic_ring.wav", 100);
    }
}

void PKC_setRGBMode(int value) {
    rgbMode = value;
}

void PKC_refresh() {

    int latchedRefreshAll = PKC_refreshAll;

    // Check for items that need to be refreshed because of a refresh zone
    for (int i = 0; i < itemCount; i++) {
        if (itemArray[i]->drawEnable && (!(latchedRefreshAll || itemArray[i]->needRefresh))
            && PKC_checkRefreshZones(itemArray[i])) {
            PKC_setRefreshFlagItemsUpperLevelel(itemArray[i]);
        }
    }

    // Check for other items that need refresh because of refresh of item bellow
    for (int i = 0; i < itemCount; i++) {
        if (itemArray[i]->needRefresh) {
            PKC_setRefreshFlagItemsUpperLevelel(itemArray[i]);
        }
    }

    // Wait here while DSI transfer is not completed
    // because it uses the DMA2D, and we need it for next steps.
    while(!dsiInterruptDone);

    if (latchedRefreshAll) {
        // Clear all screen
        BSP_LCD_Clear(PKC_theme.backGroundColor);
    } else {
        // Clear space for each refresh zone
        BSP_LCD_SetTextColor(PKC_theme.backGroundColor);
        for (int i = PKC_firstRZ; i != PKC_lastRZ; i = (i+1)%PKC_MAX_REFRESH_ZONES) {
            BSP_LCD_FillRect(PKC_refreshZones[i].x, PKC_refreshZones[i].y, PKC_refreshZones[i].width, PKC_refreshZones[i].height);
        }
    }


    // Draw each item
    for (int i = 0; i < itemCount; i++) {
        if (itemArray[i]->drawEnable) {
            if (latchedRefreshAll || itemArray[i]->needRefresh) {
                itemArray[i]->draw(itemArray[i]);
                if (itemArray[i]->needRefresh > 0) {
                    itemArray[i]->needRefresh--;
                } else if (!latchedRefreshAll) {
                    itemArray[i] = 0;
                }
            }
        }
    }

    // Print FPS indicator (or remaining mlilliseconds)
    BSP_LCD_SetTextColor(PKC_theme.backGroundColor);
    BSP_LCD_FillRect(0, 0, 100, 20);//36
    char buffer[3];
    sprintf(buffer, "%02d", PKC_FPS);
    BSP_LCD_SetTextColor(PKC_theme.color0);
    BSP_LCD_DisplayStringAt(0, 0, (uint8_t*)(buffer), LEFT_MODE);

    if (latchedRefreshAll) {
        PKC_refreshAll--;
    }
    PKC_cleanRefreshZones();

    PKC_FPS = 20-(HAL_GetTick()-tickTimer1);

    // Be sure to wait at least 20ms between each frame
    while(HAL_GetTick()-tickTimer1 < 20);
    // PKC_FPS = (3*PKC_FPS + (16000/(HAL_GetTick()-tickTimer1)))/4;
    tickTimer1 = HAL_GetTick();


    front_buffer = pend_buffer;
    // pend_buffer = 1 - front_buffer;

    /* Disable DSI Wrapper */
    __HAL_DSI_WRAPPER_DISABLE(&hdsi_eval);
    /* Update LTDC configuration */
    LTDC_LAYER(&hltdc_eval, 0)->CFBAR = ((uint32_t)Buffers[front_buffer]);
    __HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(&hltdc_eval);
    /* Enable DSI Wrapper */
    __HAL_DSI_WRAPPER_ENABLE(&hdsi_eval);

    BSP_LCD_SetFBStartAdress((uint8_t*)(Buffers[pend_buffer]));

    /* Refresh the display */
    HAL_DSI_Refresh(&hdsi_eval);
    dsiInterruptDone = 0;
}

uint32_t* PKC_getCurrentBuffer() {
    return (uint32_t *)Buffers[front_buffer];
}

extern FT6336G_point p0, p1;
void PKC_tsEvent() {
    int itemIndex0 = -1;
    int itemIndex1 = -1;
    int nbrPoints = FT6336G_getNbrPoints();
    if (nbrPoints > 0) {
        // For each item
        for (int i = itemCount-1; i >= 0; i--) {
            // If touch screen event is inside item, and item is drawn
            if (itemArray[i]->drawEnable && itemArray[i]->x <= p0.x && itemArray[i]->y <= p0.y &&
            itemArray[i]->x+itemArray[i]->width > p0.x && itemArray[i]->y+itemArray[i]->height > p0.y) {
                // If touch screen event callback of item is defined
                if (itemArray[i]->tsEvent != 0) {
                    itemIndex0 = i;
                }
                i = 0;
            }
        }
    }
    if (nbrPoints > 1) {
        // For each item
        for (int i = itemCount-1; i >= 0; i--) {
            // If touch screen event is inside item, and item is drawn
            if (itemArray[i]->drawEnable && itemArray[i]->x <= p1.x && itemArray[i]->y <= p1.y &&
            itemArray[i]->x+itemArray[i]->width > p1.x && itemArray[i]->y+itemArray[i]->height > p1.y) {
                // If touch screen event callback of item is defined
                if (itemArray[i]->tsEvent != 0) {
                    itemIndex1 = i;
                }
                i = 0;
            }
        }
    }
    if (itemIndex0 != -1) {
        if (p0.currentItemIndex != itemIndex0) {
            p0.event |= TS_PRESS_DOWN;
        }
        // Call touch screen event callback of item
        itemArray[itemIndex0]->tsEvent(itemArray[itemIndex0], &p0);
    }
    if (itemIndex1 != -1 && itemIndex0 != itemIndex1) {
        if (p1.currentItemIndex != itemIndex1) {
            p1.event |= TS_PRESS_DOWN;
        }
        // Call touch screen event callback of item
        itemArray[itemIndex1]->tsEvent(itemArray[itemIndex1], &p1);
    }
    if (nbrPoints > 0 && itemIndex0 != p0.currentItemIndex) {
        if (p0.currentItemIndex != -1 && p0.currentItemIndex < itemCount) {
            p0.event |= TS_LIFT_UP;
            itemArray[p0.currentItemIndex]->tsEvent(itemArray[p0.currentItemIndex], &p0);
        }
        p0.currentItemIndex = itemIndex0;
    }
    if (nbrPoints > 1 && itemIndex1 != p1.currentItemIndex) {
        if (p1.currentItemIndex != -1 && p1.currentItemIndex < itemCount) {
            p1.event |= TS_LIFT_UP;
            itemArray[p1.currentItemIndex]->tsEvent(itemArray[p1.currentItemIndex], &p1);
        }
        p1.currentItemIndex = itemIndex1;
    }
}


/**
  * @brief  End of Refresh DSI callback.
  * @param  hdsi: pointer to a DSI_HandleTypeDef structure that contains
  *               the configuration information for the DSI.
  * @retval None
  */
void HAL_DSI_EndOfRefreshCallback(DSI_HandleTypeDef *hdsi) {
    dsiInterruptDone = 1;
}


/**
  * @brief  Initializes the DSI LCD.
  * The ititialization is done as below:
  *     - DSI PLL ititialization
  *     - DSI ititialization
  *     - LTDC ititialization
  *     - OTM8009A LCD Display IC Driver ititialization
  * @param  None
  * @retval LCD state
  */
static uint8_t LCD_Init(void)
{
    static DSI_PHY_TimerTypeDef PhyTimings;
    static DSI_CmdCfgTypeDef CmdCfg;
    static DSI_LPCmdTypeDef LPCmd;
    static DSI_PLLInitTypeDef dsiPllInit;
    static RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

    /* Toggle Hardware Reset of the DSI LCD using
    * its XRES signal (active low) */
    BSP_LCD_Reset();

    /* Call first MSP Initialize only in case of first initialization
    * This will set IP blocks LTDC, DSI and DMA2D
    * - out of reset
    * - clocked
    * - NVIC IRQ related to IP blocks enabled
    */
    BSP_LCD_MspInit();

    /* LCD clock configuration */
    /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
    /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 417 Mhz */
    /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 417 MHz / 5 = 83.4 MHz */
    /* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_2 = 83.4 / 2 = 41.7 MHz */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 417;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    /* Base address of DSI Host/Wrapper registers to be set before calling De-Init */
    hdsi_eval.Instance = DSI;

    HAL_DSI_DeInit(&(hdsi_eval));

#if defined(USE_STM32469I_DISCO_REVA)
    dsiPllInit.PLLNDIV  = 100;
    dsiPllInit.PLLIDF   = DSI_PLL_IN_DIV5;
#else
    dsiPllInit.PLLNDIV  = 125;
    dsiPllInit.PLLIDF   = DSI_PLL_IN_DIV2;
#endif  /* USE_STM32469I_DISCO_REVA */
    dsiPllInit.PLLODF  = DSI_PLL_OUT_DIV1;

    hdsi_eval.Init.NumberOfLanes = DSI_TWO_DATA_LANES;
    hdsi_eval.Init.TXEscapeCkdiv = 0x4;
    HAL_DSI_Init(&(hdsi_eval), &(dsiPllInit));

    /* Configure the DSI for Command mode */
    CmdCfg.VirtualChannelID      = 0;
    CmdCfg.HSPolarity            = DSI_HSYNC_ACTIVE_HIGH;
    CmdCfg.VSPolarity            = DSI_VSYNC_ACTIVE_HIGH;
    CmdCfg.DEPolarity            = DSI_DATA_ENABLE_ACTIVE_HIGH;
    CmdCfg.ColorCoding           = DSI_RGB888;
    CmdCfg.CommandSize           = HACT;
    CmdCfg.TearingEffectSource   = DSI_TE_DSILINK;
    CmdCfg.TearingEffectPolarity = DSI_TE_RISING_EDGE;
    CmdCfg.VSyncPol              = DSI_VSYNC_FALLING;
    CmdCfg.AutomaticRefresh      = DSI_AR_DISABLE;
    CmdCfg.TEAcknowledgeRequest  = DSI_TE_ACKNOWLEDGE_ENABLE;
    HAL_DSI_ConfigAdaptedCommandMode(&hdsi_eval, &CmdCfg);

    LPCmd.LPGenShortWriteNoP    = DSI_LP_GSW0P_ENABLE;
    LPCmd.LPGenShortWriteOneP   = DSI_LP_GSW1P_ENABLE;
    LPCmd.LPGenShortWriteTwoP   = DSI_LP_GSW2P_ENABLE;
    LPCmd.LPGenShortReadNoP     = DSI_LP_GSR0P_ENABLE;
    LPCmd.LPGenShortReadOneP    = DSI_LP_GSR1P_ENABLE;
    LPCmd.LPGenShortReadTwoP    = DSI_LP_GSR2P_ENABLE;
    LPCmd.LPGenLongWrite        = DSI_LP_GLW_ENABLE;
    LPCmd.LPDcsShortWriteNoP    = DSI_LP_DSW0P_ENABLE;
    LPCmd.LPDcsShortWriteOneP   = DSI_LP_DSW1P_ENABLE;
    LPCmd.LPDcsShortReadNoP     = DSI_LP_DSR0P_ENABLE;
    LPCmd.LPDcsLongWrite        = DSI_LP_DLW_ENABLE;
    HAL_DSI_ConfigCommand(&hdsi_eval, &LPCmd);

    /* Configure DSI PHY HS2LP and LP2HS timings */
    PhyTimings.ClockLaneHS2LPTime = 35;
    PhyTimings.ClockLaneLP2HSTime = 35;
    PhyTimings.DataLaneHS2LPTime = 35;
    PhyTimings.DataLaneLP2HSTime = 35;
    PhyTimings.DataLaneMaxReadTime = 0;
    PhyTimings.StopWaitTime = 10;
    HAL_DSI_ConfigPhyTimer(&hdsi_eval, &PhyTimings);

    /* Initialize LTDC */
    LTDC_Init();

    /* Start DSI */
    HAL_DSI_Start(&(hdsi_eval));

#if defined (USE_STM32469I_DISCO_REVC)
    /* Initialize the NT35510 LCD Display IC Driver (3K138 LCD IC Driver) */
    NT35510_Init(NT35510_FORMAT_RGB888, LCD_ORIENTATION_LANDSCAPE);
#else
    /* Initialize the OTM8009A LCD Display IC Driver (KoD LCD IC Driver) */
    OTM8009A_Init(OTM8009A_COLMOD_RGB888, LCD_ORIENTATION_LANDSCAPE);
#endif

    LPCmd.LPGenShortWriteNoP    = DSI_LP_GSW0P_DISABLE;
    LPCmd.LPGenShortWriteOneP   = DSI_LP_GSW1P_DISABLE;
    LPCmd.LPGenShortWriteTwoP   = DSI_LP_GSW2P_DISABLE;
    LPCmd.LPGenShortReadNoP     = DSI_LP_GSR0P_DISABLE;
    LPCmd.LPGenShortReadOneP    = DSI_LP_GSR1P_DISABLE;
    LPCmd.LPGenShortReadTwoP    = DSI_LP_GSR2P_DISABLE;
    LPCmd.LPGenLongWrite        = DSI_LP_GLW_DISABLE;
    LPCmd.LPDcsShortWriteNoP    = DSI_LP_DSW0P_DISABLE;
    LPCmd.LPDcsShortWriteOneP   = DSI_LP_DSW1P_DISABLE;
    LPCmd.LPDcsShortReadNoP     = DSI_LP_DSR0P_DISABLE;
    LPCmd.LPDcsLongWrite        = DSI_LP_DLW_DISABLE;
    HAL_DSI_ConfigCommand(&hdsi_eval, &LPCmd);

    HAL_DSI_ConfigFlowControl(&hdsi_eval, DSI_FLOW_CONTROL_BTA);
    /* Refresh the display */
    HAL_DSI_Refresh(&hdsi_eval);

    return LCD_OK;
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void LTDC_Init(void)
{
    /* DeInit */
    hltdc_eval.Instance = LTDC;
    HAL_LTDC_DeInit(&hltdc_eval);

    /* LTDC Config */
    /* Timing and polarity */
    hltdc_eval.Init.HorizontalSync = HSYNC;
    hltdc_eval.Init.VerticalSync = VSYNC;
    hltdc_eval.Init.AccumulatedHBP = HSYNC+HBP;
    hltdc_eval.Init.AccumulatedVBP = VSYNC+VBP;
    hltdc_eval.Init.AccumulatedActiveH = VSYNC+VBP+VACT;
    hltdc_eval.Init.AccumulatedActiveW = HSYNC+HBP+HACT;
    hltdc_eval.Init.TotalHeigh = VSYNC+VBP+VACT+VFP;
    hltdc_eval.Init.TotalWidth = HSYNC+HBP+HACT+HFP;

    /* background value */
    hltdc_eval.Init.Backcolor.Blue = 0;
    hltdc_eval.Init.Backcolor.Green = 0;
    hltdc_eval.Init.Backcolor.Red = 0;

    /* Polarity */
    hltdc_eval.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    hltdc_eval.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    hltdc_eval.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    hltdc_eval.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    hltdc_eval.Instance = LTDC;

    HAL_LTDC_Init(&hltdc_eval);
}

/**
  * @brief  Converts a line to an ARGB8888 pixel format.
  * @param  pSrc: Pointer to source buffer
  * @param  pDst: Output color
  * @param  xSize: Buffer width
  * @param  ColorMode: Input color mode
  * @retval None
  */
void PKC_CopyBuffer(uint32_t *pSrc, uint32_t *pDst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize)
{

    uint32_t destination = (uint32_t)pDst + (y * 800 + x) * 4;
    uint32_t source      = (uint32_t)pSrc;

    /*##-1- Configure the DMA2D Mode, Color Mode and output offset #############*/
    hdma2d.Init.Mode         = DMA2D_M2M;
    hdma2d.Init.ColorMode    = DMA2D_ARGB8888;
    hdma2d.Init.OutputOffset = 800 - xsize;

    /*##-2- DMA2D Callbacks Configuration ######################################*/
    hdma2d.XferCpltCallback  = NULL;

    /*##-3- Foreground Configuration ###########################################*/
    hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[1].InputAlpha = 0xFF;
    hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
    hdma2d.LayerCfg[1].InputOffset = 0;

    hdma2d.Instance          = DMA2D;

    /* DMA2D Initialization */
    if(HAL_DMA2D_Init(&hdma2d) == HAL_OK)
    {
        if(HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK)
        {
            if (HAL_DMA2D_Start(&hdma2d, source, destination, xsize, ysize) == HAL_OK)
            {
                /* Polling For DMA transfer */
                HAL_DMA2D_PollForTransfer(&hdma2d, 100);
            }
        }
    }
}
void PKC_CopyBufferBlend(uint32_t *pSrc, uint32_t *pDst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize)
{

    uint32_t destination = (uint32_t)pDst + (y * 800 + x) * 4;
    uint32_t source      = (uint32_t)pSrc;

    /*##-1- Configure the DMA2D Mode, Color Mode and output offset #############*/
    hdma2d.Init.Mode         = DMA2D_M2M_BLEND;
    hdma2d.Init.ColorMode    = DMA2D_ARGB8888;
    hdma2d.Init.OutputOffset = 800 - xsize;

    /*##-2- DMA2D Callbacks Configuration ######################################*/
    hdma2d.XferCpltCallback  = NULL;

    /*##-3- Foreground Configuration ###########################################*/

    hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
    hdma2d.LayerCfg[1].InputOffset = 0;

    hdma2d.LayerCfg[0].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[0].InputColorMode = DMA2D_INPUT_ARGB8888;
    hdma2d.LayerCfg[0].InputOffset = 800 - xsize;

    hdma2d.Instance = DMA2D;

    /* DMA2D Initialization */
    if(HAL_DMA2D_Init(&hdma2d) == HAL_OK)
    {
        if(HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK && HAL_DMA2D_ConfigLayer(&hdma2d, 0) == HAL_OK)
        {
            if (HAL_DMA2D_BlendingStart(&hdma2d, source, destination, destination, xsize, ysize) == HAL_OK)
            {
                /* Polling For DMA transfer */
                HAL_DMA2D_PollForTransfer(&hdma2d, 100);
            }
        }
    }
}

void PKC_drawBuffer(uint32_t *pSrc, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize) {
    PKC_CopyBuffer(pSrc, (uint32_t *)Buffers[front_buffer], x, y, xsize, ysize);
}


void PKC_addRefreshZone(int x, int y, int width, int height) {
    int nextIndex = (PKC_lastRZ + 1) % PKC_MAX_REFRESH_ZONES;
    if (nextIndex != PKC_firstRZ) {
        PKC_refreshZones[PKC_lastRZ].x = x;
        PKC_refreshZones[PKC_lastRZ].y = y;
        PKC_refreshZones[PKC_lastRZ].width = width;
        PKC_refreshZones[PKC_lastRZ].height = height;
        PKC_refreshZones[PKC_lastRZ].timeToLive = 1;
        PKC_lastRZ = nextIndex;
    } else {
        printf("Error: Not enough refresh zones\r\n");
    }
}
void PKC_addRefreshZoneFromItem(PKC_item* item) {
    PKC_addRefreshZone(item->x, item->y, item->width, item->height);
}
void PKC_addRefreshZoneAll() {
    PKC_refreshAll = 1;
}

inline void PKC_setRefreshItemFlag(PKC_item* item) {
    item->needRefresh = 1;
    // if (item->needRefresh == 0) {
    //     item->needRefresh = 1;
    //     for (int i = 0; i < itemCount; i++) {
    //         if (itemArray[i] != item && itemArray[i]->drawEnable && itemArray[i]->needRefresh == 0 &&
    //         itemArray[i]->level > item->level && PKC_isItemInItem(item, itemArray[i])) {
    //             PKC_setRefreshItemFlag(itemArray[i]);
    //         }
    //     }
    // }
}


// Picture loading in RAM
void PKC_loadPicInRam(char* picPath, void* ramPointer, int bytesize) {
    FIL fil;
    if (f_open(&fil, picPath, FA_READ) == FR_OK) {
        UINT read_bytes;
        f_read(&fil, ramPointer, bytesize, &read_bytes);
        f_close(&fil);
    }
}