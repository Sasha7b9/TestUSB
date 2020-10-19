#include "Hardware/Hardware.h"
#include <stm32f2xx_hal.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    RCC_OscInitTypeDef oscIS;
    RCC_PeriphCLKInitTypeDef periphClkIS;

    __PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    oscIS.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
    oscIS.PLL.PLLState = RCC_PLL_NONE;
    oscIS.LSEState = RCC_LSE_ON;
    oscIS.LSIState = RCC_LSI_OFF;

    if (HAL_RCC_OscConfig(&oscIS) != HAL_OK)
    {
        HARDWARE_ERROR
    }

    periphClkIS.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    periphClkIS.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&periphClkIS) != HAL_OK)
    {
        HARDWARE_ERROR
    }
    __HAL_RCC_RTC_ENABLE();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
    __HAL_RCC_RTC_DISABLE();
    HAL_PWR_DisableBkUpAccess();
    __PWR_CLK_DISABLE();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{
    GPIO_InitTypeDef structGPIO =
    {
        GPIO_PIN_4,
        GPIO_MODE_ANALOG,
        GPIO_NOPULL
    };

    HAL_GPIO_Init(GPIOA, &structGPIO);

    static DMA_HandleTypeDef hdmaDAC1 =
    {
        DMA1_Stream5,
        {
            DMA_CHANNEL_7,
            DMA_MEMORY_TO_PERIPH,
            DMA_PINC_DISABLE,
            DMA_MINC_ENABLE,
            DMA_PDATAALIGN_BYTE,
            DMA_MDATAALIGN_BYTE,
            DMA_CIRCULAR,
            DMA_PRIORITY_HIGH,
            DMA_FIFOMODE_DISABLE,
            DMA_FIFO_THRESHOLD_HALFFULL,
            DMA_MBURST_SINGLE,
            DMA_PBURST_SINGLE
        }
    };

    HAL_DMA_Init(&hdmaDAC1);

    __HAL_LINKDMA(hdac, DMA_Handle1, hdmaDAC1);

    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_DAC_MspDeInit(DAC_HandleTypeDef* hdac)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc)
{
    __HAL_RCC_CRC_CLK_ENABLE();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc)
{
    __HAL_RCC_CRC_FORCE_RESET();
    __HAL_RCC_CRC_RELEASE_RESET();
}
