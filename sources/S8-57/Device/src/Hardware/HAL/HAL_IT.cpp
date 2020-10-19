#include "defines.h"
#include "Hardware/VCP.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/HAL_PINS.h"
#include <stm32f4xx_hal.h>


INTERRUPT_BEGIN



void HardFault_Handler()
{
    while (1) //-V776
    {
    }
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}


// VCP
void OTG_FS_IRQHandler()
{
    HAL_PCD_IRQHandler(static_cast<PCD_HandleTypeDef *>(VCP::handlePCD));
}


// Флешка
void OTG_HS_IRQHandler()
{
}



void NMI_Handler(void)
{
}


void MemManage_Handler()
{
    while (1)
    {
        NVIC_SystemReset();
    }
}


void BusFault_Handler()
{
    while (1)
    {
        NVIC_SystemReset();
    }
}


void UsageFault_Handler()
{
    while (1)
    {
        NVIC_SystemReset();
    }
}


void SVC_Handler(void)
{
}


void DebugMon_Handler(void)
{
}


void PendSV_Handler(void)
{
}

void HAL_GPIO_EXTI_Callback(uint16 pin)
{
}

INTERRUPT_END
