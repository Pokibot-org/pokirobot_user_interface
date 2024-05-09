/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32f4xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */
extern DMA_HandleTypeDef hdma_i2c1_rx;

extern DMA_HandleTypeDef hdma_i2c1_tx;

extern DMA_HandleTypeDef hdma_sdio_rx;

extern DMA_HandleTypeDef hdma_sdio_tx;

extern DMA_HandleTypeDef hdma_tim3_ch4_up;

extern DMA_HandleTypeDef hdma_tim8_ch3;

extern DMA_HandleTypeDef hdma_usart3_tx;

extern DMA_HandleTypeDef hdma_usart6_tx;

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
                                                            /**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
* @brief ADC MSP Initialization
* This function configures the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PC2     ------> ADC1_IN12
    PA4     ------> ADC1_IN4
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ADC1 interrupt Init */
    HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);
  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }

}

/**
* @brief ADC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PC2     ------> ADC1_IN12
    PA4     ------> ADC1_IN4
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);

    /* ADC1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(ADC_IRQn);
  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }

}

/**
* @brief I2C MSP Initialization
* This function configures the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = I2C1_SCL_Pin|I2C1_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();

    /* I2C1 DMA Init */
    /* I2C1_RX Init */
    hdma_i2c1_rx.Instance = DMA1_Stream0;
    hdma_i2c1_rx.Init.Channel = DMA_CHANNEL_1;
    hdma_i2c1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_i2c1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_i2c1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_i2c1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_i2c1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_i2c1_rx.Init.Mode = DMA_NORMAL;
    hdma_i2c1_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_i2c1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_i2c1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hi2c,hdmarx,hdma_i2c1_rx);

    /* I2C1_TX Init */
    hdma_i2c1_tx.Instance = DMA1_Stream6;
    hdma_i2c1_tx.Init.Channel = DMA_CHANNEL_1;
    hdma_i2c1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_i2c1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_i2c1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_i2c1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_i2c1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_i2c1_tx.Init.Mode = DMA_NORMAL;
    hdma_i2c1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_i2c1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_i2c1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hi2c,hdmatx,hdma_i2c1_tx);

    /* I2C1 interrupt Init */
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
  else if(hi2c->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspInit 0 */

  /* USER CODE END I2C2_MspInit 0 */

    __HAL_RCC_GPIOH_CLK_ENABLE();
    /**I2C2 GPIO Configuration
    PH4     ------> I2C2_SCL
    PH5     ------> I2C2_SDA
    */
    GPIO_InitStruct.Pin = I2C2_SCL_Pin|I2C2_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();
    /* I2C2 interrupt Init */
    HAL_NVIC_SetPriority(I2C2_EV_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
  /* USER CODE BEGIN I2C2_MspInit 1 */

  /* USER CODE END I2C2_MspInit 1 */
  }

}

/**
* @brief I2C MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(I2C1_SCL_GPIO_Port, I2C1_SCL_Pin);

    HAL_GPIO_DeInit(I2C1_SDA_GPIO_Port, I2C1_SDA_Pin);

    /* I2C1 DMA DeInit */
    HAL_DMA_DeInit(hi2c->hdmarx);
    HAL_DMA_DeInit(hi2c->hdmatx);

    /* I2C1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
  else if(hi2c->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspDeInit 0 */

  /* USER CODE END I2C2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C2_CLK_DISABLE();

    /**I2C2 GPIO Configuration
    PH4     ------> I2C2_SCL
    PH5     ------> I2C2_SDA
    */
    HAL_GPIO_DeInit(I2C2_SCL_GPIO_Port, I2C2_SCL_Pin);

    HAL_GPIO_DeInit(I2C2_SDA_GPIO_Port, I2C2_SDA_Pin);

    /* I2C2 interrupt DeInit */
    HAL_NVIC_DisableIRQ(I2C2_EV_IRQn);
  /* USER CODE BEGIN I2C2_MspDeInit 1 */

  /* USER CODE END I2C2_MspDeInit 1 */
  }

}

/**
* @brief SD MSP Initialization
* This function configures the hardware resources used in this example
* @param hsd: SD handle pointer
* @retval None
*/
void HAL_SD_MspInit(SD_HandleTypeDef* hsd)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hsd->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspInit 0 */

  /* USER CODE END SDIO_MspInit 0 */
  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CLK48|RCC_PERIPHCLK_SDIO;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 144;
    PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV6;
    PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLSAIP;
    PeriphClkInitStruct.SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_SDIO_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SDIO GPIO Configuration
    PC12     ------> SDIO_CK
    PC11     ------> SDIO_D3
    PC10     ------> SDIO_D2
    PD2     ------> SDIO_CMD
    PC9     ------> SDIO_D1
    PC8     ------> SDIO_D0
    */
    GPIO_InitStruct.Pin = uSD_CLK_Pin|uSD_D3_Pin|uSD_D2_Pin|uSD_D1_Pin
                          |uSD_D0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = uSD_CMD_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(uSD_CMD_GPIO_Port, &GPIO_InitStruct);

    /* SDIO DMA Init */
    /* SDIO_RX Init */
    hdma_sdio_rx.Instance = DMA2_Stream3;
    hdma_sdio_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdio_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sdio_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio_rx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio_rx.Init.Mode = DMA_PFCTRL;
    hdma_sdio_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_sdio_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdio_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdio_rx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdio_rx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdio_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hsd,hdmarx,hdma_sdio_rx);

    /* SDIO_TX Init */
    hdma_sdio_tx.Instance = DMA2_Stream6;
    hdma_sdio_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdio_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sdio_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio_tx.Init.Mode = DMA_PFCTRL;
    hdma_sdio_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_sdio_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdio_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdio_tx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdio_tx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdio_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hsd,hdmatx,hdma_sdio_tx);

    /* SDIO interrupt Init */
    HAL_NVIC_SetPriority(SDIO_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(SDIO_IRQn);
  /* USER CODE BEGIN SDIO_MspInit 1 */

  /* USER CODE END SDIO_MspInit 1 */
  }

}

/**
* @brief SD MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hsd: SD handle pointer
* @retval None
*/
void HAL_SD_MspDeInit(SD_HandleTypeDef* hsd)
{
  if(hsd->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspDeInit 0 */

  /* USER CODE END SDIO_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDIO_CLK_DISABLE();

    /**SDIO GPIO Configuration
    PC12     ------> SDIO_CK
    PC11     ------> SDIO_D3
    PC10     ------> SDIO_D2
    PD2     ------> SDIO_CMD
    PC9     ------> SDIO_D1
    PC8     ------> SDIO_D0
    */
    HAL_GPIO_DeInit(GPIOC, uSD_CLK_Pin|uSD_D3_Pin|uSD_D2_Pin|uSD_D1_Pin
                          |uSD_D0_Pin);

    HAL_GPIO_DeInit(uSD_CMD_GPIO_Port, uSD_CMD_Pin);

    /* SDIO DMA DeInit */
    HAL_DMA_DeInit(hsd->hdmarx);
    HAL_DMA_DeInit(hsd->hdmatx);

    /* SDIO interrupt DeInit */
    HAL_NVIC_DisableIRQ(SDIO_IRQn);
  /* USER CODE BEGIN SDIO_MspDeInit 1 */

  /* USER CODE END SDIO_MspDeInit 1 */
  }

}

/**
* @brief TIM_Base MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspInit 0 */

  /* USER CODE END TIM1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM1_CLK_ENABLE();
  /* USER CODE BEGIN TIM1_MspInit 1 */

  /* USER CODE END TIM1_MspInit 1 */
  }
  else if(htim_base->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();

    /* TIM3 DMA Init */
    /* TIM3_CH4_UP Init */
    hdma_tim3_ch4_up.Instance = DMA1_Stream2;
    hdma_tim3_ch4_up.Init.Channel = DMA_CHANNEL_5;
    hdma_tim3_ch4_up.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_tim3_ch4_up.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim3_ch4_up.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim3_ch4_up.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_tim3_ch4_up.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_tim3_ch4_up.Init.Mode = DMA_NORMAL;
    hdma_tim3_ch4_up.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tim3_ch4_up.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_tim3_ch4_up) != HAL_OK)
    {
      Error_Handler();
    }

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one stream to perform all the requested DMAs. */
    __HAL_LINKDMA(htim_base,hdma[TIM_DMA_ID_CC4],hdma_tim3_ch4_up);
    __HAL_LINKDMA(htim_base,hdma[TIM_DMA_ID_UPDATE],hdma_tim3_ch4_up);

    /* TIM3 interrupt Init */
    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
  /* USER CODE BEGIN TIM3_MspInit 1 */

  /* USER CODE END TIM3_MspInit 1 */
  }
  else if(htim_base->Instance==TIM8)
  {
  /* USER CODE BEGIN TIM8_MspInit 0 */

  /* USER CODE END TIM8_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM8_CLK_ENABLE();

    /* TIM8 DMA Init */
    /* TIM8_CH3 Init */
    hdma_tim8_ch3.Instance = DMA2_Stream4;
    hdma_tim8_ch3.Init.Channel = DMA_CHANNEL_7;
    hdma_tim8_ch3.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim8_ch3.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim8_ch3.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim8_ch3.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_tim8_ch3.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_tim8_ch3.Init.Mode = DMA_CIRCULAR;
    hdma_tim8_ch3.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tim8_ch3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_tim8_ch3) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(htim_base,hdma[TIM_DMA_ID_CC3],hdma_tim8_ch3);

  /* USER CODE BEGIN TIM8_MspInit 1 */

  /* USER CODE END TIM8_MspInit 1 */
  }

}

/**
* @brief TIM_PWM MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_pwm: TIM_PWM handle pointer
* @retval None
*/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm)
{
  if(htim_pwm->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspInit 0 */

  /* USER CODE END TIM4_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();
  /* USER CODE BEGIN TIM4_MspInit 1 */

  /* USER CODE END TIM4_MspInit 1 */
  }

}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(htim->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspPostInit 0 */

  /* USER CODE END TIM3_MspPostInit 0 */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM3 GPIO Configuration
    PB1     ------> TIM3_CH4
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM3_MspPostInit 1 */

  /* USER CODE END TIM3_MspPostInit 1 */
  }
  else if(htim->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspPostInit 0 */

  /* USER CODE END TIM4_MspPostInit 0 */

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**TIM4 GPIO Configuration
    PD13     ------> TIM4_CH2
    */
    GPIO_InitStruct.Pin = MIC_CK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(MIC_CK_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM4_MspPostInit 1 */

  /* USER CODE END TIM4_MspPostInit 1 */
  }
  else if(htim->Instance==TIM8)
  {
  /* USER CODE BEGIN TIM8_MspPostInit 0 */

  /* USER CODE END TIM8_MspPostInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM8 GPIO Configuration
    PB15     ------> TIM8_CH3N
    */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM8_MspPostInit 1 */

  /* USER CODE END TIM8_MspPostInit 1 */
  }

}
/**
* @brief TIM_Base MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspDeInit 0 */

  /* USER CODE END TIM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM1_CLK_DISABLE();
  /* USER CODE BEGIN TIM1_MspDeInit 1 */

  /* USER CODE END TIM1_MspDeInit 1 */
  }
  else if(htim_base->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();

    /* TIM3 DMA DeInit */
    HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC4]);
    HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_UPDATE]);

    /* TIM3 interrupt DeInit */
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }
  else if(htim_base->Instance==TIM8)
  {
  /* USER CODE BEGIN TIM8_MspDeInit 0 */

  /* USER CODE END TIM8_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM8_CLK_DISABLE();

    /* TIM8 DMA DeInit */
    HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC3]);
  /* USER CODE BEGIN TIM8_MspDeInit 1 */

  /* USER CODE END TIM8_MspDeInit 1 */
  }

}

/**
* @brief TIM_PWM MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_pwm: TIM_PWM handle pointer
* @retval None
*/
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim_pwm)
{
  if(htim_pwm->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspDeInit 0 */

  /* USER CODE END TIM4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM4_CLK_DISABLE();
  /* USER CODE BEGIN TIM4_MspDeInit 1 */

  /* USER CODE END TIM4_MspDeInit 1 */
  }

}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = STLK_RX_Pin|STLK_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 DMA Init */
    /* USART3_TX Init */
    hdma_usart3_tx.Instance = DMA1_Stream3;
    hdma_usart3_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_tx.Init.Mode = DMA_NORMAL;
    hdma_usart3_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart3_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart3_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(huart,hdmatx,hdma_usart3_tx);

  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
  else if(huart->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspInit 0 */

  /* USER CODE END USART6_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART6_CLK_ENABLE();

    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**USART6 GPIO Configuration
    PG14     ------> USART6_TX
    PG9     ------> USART6_RX
    */
    GPIO_InitStruct.Pin = ARDUINO_USART6_TX_Pin|USART6_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* USART6 DMA Init */
    /* USART6_TX Init */
    hdma_usart6_tx.Instance = DMA2_Stream7;
    hdma_usart6_tx.Init.Channel = DMA_CHANNEL_5;
    hdma_usart6_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart6_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart6_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart6_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart6_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart6_tx.Init.Mode = DMA_NORMAL;
    hdma_usart6_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart6_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart6_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(huart,hdmatx,hdma_usart6_tx);

    /* USART6 interrupt Init */
    HAL_NVIC_SetPriority(USART6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART6_IRQn);
  /* USER CODE BEGIN USART6_MspInit 1 */

  /* USER CODE END USART6_MspInit 1 */
  }

}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOB, STLK_RX_Pin|STLK_TX_Pin);

    /* USART3 DMA DeInit */
    HAL_DMA_DeInit(huart->hdmatx);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
  else if(huart->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspDeInit 0 */

  /* USER CODE END USART6_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART6_CLK_DISABLE();

    /**USART6 GPIO Configuration
    PG14     ------> USART6_TX
    PG9     ------> USART6_RX
    */
    HAL_GPIO_DeInit(GPIOG, ARDUINO_USART6_TX_Pin|USART6_RX_Pin);

    /* USART6 DMA DeInit */
    HAL_DMA_DeInit(huart->hdmatx);

    /* USART6 interrupt DeInit */
    HAL_NVIC_DisableIRQ(USART6_IRQn);
  /* USER CODE BEGIN USART6_MspDeInit 1 */

  /* USER CODE END USART6_MspDeInit 1 */
  }

}

static uint32_t FMC_Initialized = 0;

static void HAL_FMC_MspInit(void){
  /* USER CODE BEGIN FMC_MspInit 0 */

  /* USER CODE END FMC_MspInit 0 */
  GPIO_InitTypeDef GPIO_InitStruct ={0};
  if (FMC_Initialized) {
    return;
  }
  FMC_Initialized = 1;

  /* Peripheral clock enable */
  __HAL_RCC_FMC_CLK_ENABLE();

  /** FMC GPIO Configuration
  PE1   ------> FMC_NBL1
  PE0   ------> FMC_NBL0
  PG15   ------> FMC_SDNCAS
  PD0   ------> FMC_D2
  PI4   ------> FMC_NBL2
  PD1   ------> FMC_D3
  PI3   ------> FMC_D27
  PI2   ------> FMC_D26
  PF0   ------> FMC_A0
  PI5   ------> FMC_NBL3
  PI7   ------> FMC_D29
  PI10   ------> FMC_D31
  PI6   ------> FMC_D28
  PH15   ------> FMC_D23
  PI1   ------> FMC_D25
  PF1   ------> FMC_A1
  PI9   ------> FMC_D30
  PH13   ------> FMC_D21
  PH14   ------> FMC_D22
  PI0   ------> FMC_D24
  PF2   ------> FMC_A2
  PF3   ------> FMC_A3
  PG8   ------> FMC_SDCLK
  PF4   ------> FMC_A4
  PH3   ------> FMC_SDNE0
  PF5   ------> FMC_A5
  PH2   ------> FMC_SDCKE0
  PD15   ------> FMC_D1
  PD10   ------> FMC_D15
  PD14   ------> FMC_D0
  PD9   ------> FMC_D14
  PD8   ------> FMC_D13
  PC0   ------> FMC_SDNWE
  PF12   ------> FMC_A6
  PG1   ------> FMC_A11
  PF15   ------> FMC_A9
  PH12   ------> FMC_D20
  PF13   ------> FMC_A7
  PG0   ------> FMC_A10
  PE8   ------> FMC_D5
  PG5   ------> FMC_BA1
  PG4   ------> FMC_BA0
  PH9   ------> FMC_D17
  PH11   ------> FMC_D19
  PF14   ------> FMC_A8
  PF11   ------> FMC_SDNRAS
  PE9   ------> FMC_D6
  PE11   ------> FMC_D8
  PE14   ------> FMC_D11
  PH8   ------> FMC_D16
  PH10   ------> FMC_D18
  PE7   ------> FMC_D4
  PE10   ------> FMC_D7
  PE12   ------> FMC_D9
  PE15   ------> FMC_D12
  PE13   ------> FMC_D10
  */
  GPIO_InitStruct.Pin = FMC_NBL1_Pin|FMC_NBL0_Pin|D5_Pin|D6_Pin
                          |D8_Pin|D11_Pin|D4_Pin|D7_Pin
                          |D9_Pin|D12_Pin|D10_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SDNCAS_Pin|SDCLK_Pin|A11_Pin|A10_Pin
                          |GPIO_PIN_5|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = D2_Pin|D3_Pin|D1_Pin|D15_Pin
                          |D0_Pin|D14_Pin|D13_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = FMC_NBL2_Pin|D27_Pin|D26_Pin|FMC_NBL3_Pin
                          |D29_Pin|D31_Pin|D28_Pin|D25_Pin
                          |D30_Pin|D24_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = A0_Pin|A1_Pin|A2_Pin|A3_Pin
                          |A4_Pin|A5_Pin|A6_Pin|A9_Pin
                          |A7_Pin|A8_Pin|SDNMT48LC4M32B2B5_6A_RAS_RAS___Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = D23_Pin|D21_Pin|D22_Pin|SDNE0_Pin
                          |SDCKE0_Pin|D20_Pin|D17_Pin|D19_Pin
                          |D16_Pin|D18_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SDNWE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(SDNWE_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN FMC_MspInit 1 */

  /* USER CODE END FMC_MspInit 1 */
}

void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef* hsdram){
  /* USER CODE BEGIN SDRAM_MspInit 0 */

  /* USER CODE END SDRAM_MspInit 0 */
  HAL_FMC_MspInit();
  /* USER CODE BEGIN SDRAM_MspInit 1 */

  /* USER CODE END SDRAM_MspInit 1 */
}

static uint32_t FMC_DeInitialized = 0;

static void HAL_FMC_MspDeInit(void){
  /* USER CODE BEGIN FMC_MspDeInit 0 */

  /* USER CODE END FMC_MspDeInit 0 */
  if (FMC_DeInitialized) {
    return;
  }
  FMC_DeInitialized = 1;
  /* Peripheral clock enable */
  __HAL_RCC_FMC_CLK_DISABLE();

  /** FMC GPIO Configuration
  PE1   ------> FMC_NBL1
  PE0   ------> FMC_NBL0
  PG15   ------> FMC_SDNCAS
  PD0   ------> FMC_D2
  PI4   ------> FMC_NBL2
  PD1   ------> FMC_D3
  PI3   ------> FMC_D27
  PI2   ------> FMC_D26
  PF0   ------> FMC_A0
  PI5   ------> FMC_NBL3
  PI7   ------> FMC_D29
  PI10   ------> FMC_D31
  PI6   ------> FMC_D28
  PH15   ------> FMC_D23
  PI1   ------> FMC_D25
  PF1   ------> FMC_A1
  PI9   ------> FMC_D30
  PH13   ------> FMC_D21
  PH14   ------> FMC_D22
  PI0   ------> FMC_D24
  PF2   ------> FMC_A2
  PF3   ------> FMC_A3
  PG8   ------> FMC_SDCLK
  PF4   ------> FMC_A4
  PH3   ------> FMC_SDNE0
  PF5   ------> FMC_A5
  PH2   ------> FMC_SDCKE0
  PD15   ------> FMC_D1
  PD10   ------> FMC_D15
  PD14   ------> FMC_D0
  PD9   ------> FMC_D14
  PD8   ------> FMC_D13
  PC0   ------> FMC_SDNWE
  PF12   ------> FMC_A6
  PG1   ------> FMC_A11
  PF15   ------> FMC_A9
  PH12   ------> FMC_D20
  PF13   ------> FMC_A7
  PG0   ------> FMC_A10
  PE8   ------> FMC_D5
  PG5   ------> FMC_BA1
  PG4   ------> FMC_BA0
  PH9   ------> FMC_D17
  PH11   ------> FMC_D19
  PF14   ------> FMC_A8
  PF11   ------> FMC_SDNRAS
  PE9   ------> FMC_D6
  PE11   ------> FMC_D8
  PE14   ------> FMC_D11
  PH8   ------> FMC_D16
  PH10   ------> FMC_D18
  PE7   ------> FMC_D4
  PE10   ------> FMC_D7
  PE12   ------> FMC_D9
  PE15   ------> FMC_D12
  PE13   ------> FMC_D10
  */
  HAL_GPIO_DeInit(GPIOE, FMC_NBL1_Pin|FMC_NBL0_Pin|D5_Pin|D6_Pin
                          |D8_Pin|D11_Pin|D4_Pin|D7_Pin
                          |D9_Pin|D12_Pin|D10_Pin);

  HAL_GPIO_DeInit(GPIOG, SDNCAS_Pin|SDCLK_Pin|A11_Pin|A10_Pin
                          |GPIO_PIN_5|GPIO_PIN_4);

  HAL_GPIO_DeInit(GPIOD, D2_Pin|D3_Pin|D1_Pin|D15_Pin
                          |D0_Pin|D14_Pin|D13_Pin);

  HAL_GPIO_DeInit(GPIOI, FMC_NBL2_Pin|D27_Pin|D26_Pin|FMC_NBL3_Pin
                          |D29_Pin|D31_Pin|D28_Pin|D25_Pin
                          |D30_Pin|D24_Pin);

  HAL_GPIO_DeInit(GPIOF, A0_Pin|A1_Pin|A2_Pin|A3_Pin
                          |A4_Pin|A5_Pin|A6_Pin|A9_Pin
                          |A7_Pin|A8_Pin|SDNMT48LC4M32B2B5_6A_RAS_RAS___Pin);

  HAL_GPIO_DeInit(GPIOH, D23_Pin|D21_Pin|D22_Pin|SDNE0_Pin
                          |SDCKE0_Pin|D20_Pin|D17_Pin|D19_Pin
                          |D16_Pin|D18_Pin);

  HAL_GPIO_DeInit(SDNWE_GPIO_Port, SDNWE_Pin);

  /* USER CODE BEGIN FMC_MspDeInit 1 */

  /* USER CODE END FMC_MspDeInit 1 */
}

void HAL_SDRAM_MspDeInit(SDRAM_HandleTypeDef* hsdram){
  /* USER CODE BEGIN SDRAM_MspDeInit 0 */

  /* USER CODE END SDRAM_MspDeInit 0 */
  HAL_FMC_MspDeInit();
  /* USER CODE BEGIN SDRAM_MspDeInit 1 */

  /* USER CODE END SDRAM_MspDeInit 1 */
}

extern DMA_HandleTypeDef hdma_sai1_a;

static uint32_t SAI1_client =0;

void HAL_SAI_MspInit(SAI_HandleTypeDef* hsai)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
/* SAI1 */
    if(hsai->Instance==SAI1_Block_A)
    {
    /* Peripheral clock enable */
  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI_PLLI2S;
    PeriphClkInitStruct.PLLI2S.PLLI2SN = 429;
    PeriphClkInitStruct.PLLI2S.PLLI2SQ = 2;
    PeriphClkInitStruct.PLLI2SDivQ = 19;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    if (SAI1_client == 0)
    {
       __HAL_RCC_SAI1_CLK_ENABLE();

    /* Peripheral interrupt init*/
    HAL_NVIC_SetPriority(SAI1_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(SAI1_IRQn);
    }
    SAI1_client ++;

    /**SAI1_A_Block_A GPIO Configuration
    PE4     ------> SAI1_FS_A
    PE5     ------> SAI1_SCK_A
    PE6     ------> SAI1_SD_A
    PG7     ------> SAI1_MCLK_A
    */
    GPIO_InitStruct.Pin = SAI1_FSA_Pin|SAI1_SCKA_Pin|MIC_DATA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_SAI1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SAI1_MCLKA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_SAI1;
    HAL_GPIO_Init(SAI1_MCLKA_GPIO_Port, &GPIO_InitStruct);

      /* Peripheral DMA init*/

    hdma_sai1_a.Instance = DMA2_Stream1;
    hdma_sai1_a.Init.Channel = DMA_CHANNEL_0;
    hdma_sai1_a.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sai1_a.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sai1_a.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sai1_a.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_sai1_a.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_sai1_a.Init.Mode = DMA_NORMAL;
    hdma_sai1_a.Init.Priority = DMA_PRIORITY_LOW;
    hdma_sai1_a.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sai1_a.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sai1_a.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_sai1_a.Init.PeriphBurst = DMA_PBURST_SINGLE;
    if (HAL_DMA_Init(&hdma_sai1_a) != HAL_OK)
    {
      Error_Handler();
    }

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one stream to perform all the requested DMAs. */
    __HAL_LINKDMA(hsai,hdmarx,hdma_sai1_a);

    __HAL_LINKDMA(hsai,hdmatx,hdma_sai1_a);

    }
}

void HAL_SAI_MspDeInit(SAI_HandleTypeDef* hsai)
{
/* SAI1 */
    if(hsai->Instance==SAI1_Block_A)
    {
    SAI1_client --;
    if (SAI1_client == 0)
      {
      /* Peripheral clock disable */
       __HAL_RCC_SAI1_CLK_DISABLE();
      /* SAI1 interrupt DeInit */
      HAL_NVIC_DisableIRQ(SAI1_IRQn);
      }

    /**SAI1_A_Block_A GPIO Configuration
    PE4     ------> SAI1_FS_A
    PE5     ------> SAI1_SCK_A
    PE6     ------> SAI1_SD_A
    PG7     ------> SAI1_MCLK_A
    */
    HAL_GPIO_DeInit(GPIOE, SAI1_FSA_Pin|SAI1_SCKA_Pin|MIC_DATA_Pin);

    HAL_GPIO_DeInit(SAI1_MCLKA_GPIO_Port, SAI1_MCLKA_Pin);

    /* SAI1 DMA Deinit */
    HAL_DMA_DeInit(hsai->hdmarx);
    HAL_DMA_DeInit(hsai->hdmatx);
    }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
