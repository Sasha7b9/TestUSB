#include "Hardware/Hardware.h"
#include <stm32f4xx_hal.h>


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
