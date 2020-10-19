#pragma once

#define PRIORITY_SOUND_DMA1_STREAM5 5, 0

#define ERROR_HANDLER()             ::HAL::ErrorHandler(__FILE__, __LINE__)


#define HAL_BUS_CONFIGURE_TO_FSMC() if(HAL_BUS::mode != HAL_BUS::Mode::FSMC) { HAL_BUS::ConfigureToFSMC(); }


typedef struct _USBH_HandleTypeDef USBH_HandleTypeDef;


#ifdef WIN32
#define HAL_IWDG_REFRESH()
#else
#define HAL_IWDG_REFRESH()  (*(reinterpret_cast<uint *>(0x40003000U)) = IWDG_KEY_RELOAD)            // Вынесено в #define для уменьшения задержки
#endif


struct HAL
{
    static void Init();

    static void ErrorHandler(pCHAR, int);

    static void Delay(uint timeMS);

    static uint TimeMS();
};


// Используется для работы с питанием
struct HAL_ADC1
{
    static void Init();

    // Читает АЦП батареи
    static uint ValueBattery();

    // Читает АЦП зарядного устройства
    static uint ValueCharger();
};


// Исользуется в режиме рандомизатора
struct HAL_ADC3
{
    static void Init();
    static uint16 ValueRandomizer();
};


struct HAL_DAC1
{
    static void Init();
    static void StartDMA(void *points, uint numPoints);
    static void StopDMA();
    static void ConfigTIM7(uint16 prescaler, uint16 period);
};


struct HAL_DAC2
{
    static void Init();
    static void SetValue(uint value);
};


struct HAL_NVIC
{
    static uint irqEXTI1;
    static uint irqEXTI9_5;
    static void EnableIRQ(uint irq);
    static void DisableIRQ(uint irq);
    static void SetPriorityIRQ(uint irq, uint main, uint sub);
    static void SystemReset();
};


struct HAL_OTP
{
    // Возвращает число свободных мест для записи. Если 0, то места в OTP уже не осталось.
    static int GetSerialNumber(char buffer[17]);
    static bool SaveSerialNumber(char *servialNumber);
};


struct HAL_PCD
{
    static void Init();
};


struct HAL_TIM2
{
    static uint TimeUS();
    static uint TimeTicks();
    static void Init(uint prescaler, uint period);
    static void DeInit();
    static void Start();
    static void Stop();
    static void StartMultiMeasurement();
};


struct HAL_TIM3
{
    static void Init(uint prescaler, uint period);
    static void DeInit();
    static void Start();
    static void Stop();
    static void EnableIRQ(uint mainPriority, uint subPriority);
    static void DisableIRQ();
    static void StartIT(uint period);
    static void StopIT();
};


// Используется для выключения прибора в случае зависания
struct HAL_TIM5
{
    static void Init();
    static void ElapsedCallback();
};


struct HAL_USART3
{
    static void Init(pFuncVV recvCallback);
    static void Transmit(void *buffer, int size, uint timeout);
    static void StartReceiveIT(void *buffer, int size);
};
