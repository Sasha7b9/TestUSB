#include "defines.h"
#include "log.h"
#include "device.h"
#include <stm32f4xx.h>
#include "FPGA.h"
#include "AD9286.h"
#include "Data/DataStorage.h"
#include "Display/Display.h"
#include "Hardware/CPU.h"
#include "Hardware/FSMC.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Utils/Buffer.h"
#include "Utils/Values.h"
#include <stdlib.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FPGA::Init()
{
    givingStart = false;

    GPIO_Init();

    SetPin(Pin::SPI3_CS1);
    SetPin(Pin::SPI3_CS2);

    ResetPin(Pin::SPI3_SCK);
    ResetPin(Pin::SPI3_DAT);

    AD9286::Init();

    // ������ �������� ���� ��� �� �������������
    // PF10 - ADC3 IN8
    // PB11 - EXTI11
    // ����� ������:
    // - ��������� �� 1 ����������� ������
    // - ��������� ��������� �� ������ �������� ������� (���������� �� PB11)

    __ADC3_CLK_ENABLE();

    static GPIO_InitTypeDef isGPIOadc =
    {
        GPIO_PIN_10,
        GPIO_MODE_ANALOG,
        GPIO_NOPULL
    };
    HAL_GPIO_Init(GPIOF, &isGPIOadc);

    static GPIO_InitTypeDef isGPIOexti =
    {
        GPIO_PIN_11,
        GPIO_MODE_IT_RISING,
        GPIO_NOPULL
    };
    HAL_GPIO_Init(GPIOB, &isGPIOexti);

    HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);

    handleADC.Instance = ADC3;
    handleADC.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
    handleADC.Init.Resolution = ADC_RESOLUTION12b;
    handleADC.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    handleADC.Init.ScanConvMode = DISABLE;
    handleADC.Init.EOCSelection = ENABLE;
    handleADC.Init.ContinuousConvMode = DISABLE;
    handleADC.Init.DMAContinuousRequests = DISABLE;
    handleADC.Init.NbrOfConversion = 1;
    handleADC.Init.DiscontinuousConvMode = DISABLE;
    handleADC.Init.NbrOfDiscConversion = 0;
    handleADC.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    handleADC.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_Ext_IT11;

    if (HAL_ADC_Init(&handleADC) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    ADC_ChannelConfTypeDef sConfig;
    sConfig.Channel = ADC_CHANNEL_8;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
    sConfig.Offset = 0;

    if (HAL_ADC_ConfigChannel(&handleADC, &sConfig) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    if (HAL_ADC_Start_IT(&handleADC) != HAL_OK)
    {
        ERROR_HANDLER();
    }

    DataStorage::Init();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadSettings()
{
    hardware.LoadRanges();
    hardware.LoadRShift(Chan::A);
    hardware.LoadRShift(Chan::B);
    hardware.LoadTrigSourceInput();
    hardware.LoadTrigLev();
    hardware.LoadTBase();
    hardware.LoadTShift();
    hardware.LoadCalibratorMode();
    LoadHoldfOff();

    isRunning = false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::LoadHoldfOff()
{
    FSMC::WriteToFPGA8(WR_TRIG_HOLD_ENABLE, TRIG_HOLDOFF_ENABLED ? 1U : 0U);

    uint value = (uint)(0 - TRIG_HOLDOFF + 1);

    BitSet32 bs(value);

    LOG_WRITE("%x", bs.word);

    FSMC::WriteToFPGA8(WR_TRIG_HOLD_VALUE_LOW, bs.byte0);
    FSMC::WriteToFPGA8(WR_TRIG_HOLD_VALUE_MID, bs.byte1);
    FSMC::WriteToFPGA8(WR_TRIG_HOLD_VALUE_HI, bs.byte2);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HardwareFPGA::LoadRShift(Chan::E ch)
{
    LAST_AFFECTED_CH = ch;

    static const uint16 mask[2] = { 0x2000, 0x6000 };

    uint16 shift = SET_RSHIFT(ch);

    if (Chan(ch).IsA() && Device::CurrentMode() == Device::Mode::Tester)
    {
        shift = (uint16)((int)shift - Tester::DeltaRShiftA());
    }

    FPGA::WriteRegisters(Pin::SPI3_CS1, (uint16)(mask[ch] | (shift << 2)));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HardwareFPGA::LoadRanges()
{
    uint16 value = (uint16)(ValueForRange(Chan::B) + (ValueForRange(Chan::A) << 8));

    FPGA::WriteRegisters(Pin::SPI3_CS2, value);

    PAUSE_ON_MS(10);                // �������� �����, ����� ���������� ���� ������ ����������

    FPGA::WriteRegisters(Pin::SPI3_CS2, 0);    // ���������� ����, ����� ���� �� ���������� �������

    DEF_STRUCT(StructRange, uint8) vals[Range::Number] =
    {
        StructRange(BIN_U8(00000000)),  // 2mV      // -V2501
        StructRange(BIN_U8(00000001)),  // 5mV      // -V2501
        StructRange(BIN_U8(00000010)),  // 10mV     // -V2501
        StructRange(BIN_U8(00000011)),  // 20mV     // -V2501
        StructRange(BIN_U8(00000001)),  // 50mV     // -V2501
        StructRange(BIN_U8(00000010)),  // 100mV    // -V2501
        StructRange(BIN_U8(00000011)),  // 200mV    // -V2501
        StructRange(BIN_U8(00000001)),  // 500mV    // -V2501
        StructRange(BIN_U8(00000010)),  // 1V       // -V2501
        StructRange(BIN_U8(00000011)),  // 2V       // -V2501
        StructRange(BIN_U8(00000001)),  // 5V       // -V2501
        StructRange(BIN_U8(00000010)),  // 10V      // -V2501
        StructRange(BIN_U8(00000011))   // 20V      // -V2501
    };

    uint8 valueA = vals[SET_RANGE_A].val;

    FPGA::WritePin(Pin::A1, _GET_BIT(valueA, 1));
    FPGA::WritePin(Pin::A2, _GET_BIT(valueA, 0));

    uint8 valueB = vals[SET_RANGE_B].val;

    FPGA::WritePin(Pin::A3, _GET_BIT(valueB, 1));
    FPGA::WritePin(Pin::A4, _GET_BIT(valueB, 0));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FPGA::ReadData()
{
    Buffer dataA(FPGA::NumPoints());    // -V656
    Buffer dataB(FPGA::NumPoints());    // -V656

    ReadDataChanenl(Chan::A, dataA.Data());
    ReadDataChanenl(Chan::B, dataB.Data());

    DataSettings ds;
    ds.Fill(dataA.Data(), dataB.Data());

    DataStorage::Push(&ds);
}
