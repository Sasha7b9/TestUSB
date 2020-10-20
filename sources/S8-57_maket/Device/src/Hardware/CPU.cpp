#include "stdafx.h"
#ifndef WIN32
#include <stm32f4xx.h>
#include "defines.h"
#include "CPU.h"
#include "Hardware/Timer.h"
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define VALUE_FOR_RTC 0x644


#define RTC_CLOCK_SOURCE_LSE
// #define RTC_CLOCK_SOURCE_LSI


#ifdef RTC_CLOCK_SOURCE_LSI
#define RTC_ASYNCH_PREDIV 0x7f
#define RTC_SYNCH_PREDIV 0x0130
#endif

#ifdef RTC_CLOCK_SOURCE_LSE
#define RTC_ASYNCH_PREDIV 0x7f
#define RTC_SYNCH_PREDIV 0x00ff
#endif

static RTC_HandleTypeDef rtcHandle =
{
    RTC,
    {
        RTC_HOURFORMAT_24,          // HourFormat
        RTC_ASYNCH_PREDIV,          // AsynchPrediv
        RTC_SYNCH_PREDIV,           // SynchPrediv
        RTC_OUTPUT_DISABLE,         // OutPut
        RTC_OUTPUT_POLARITY_HIGH,   // OutPutPolarity
        RTC_OUTPUT_TYPE_OPENDRAIN   // OutPutType
    }
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPU::Init()
{
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CPU::RTC_::SetCorrection(int8)
{
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool CPU::RTC_::SetTimeAndData(int8 day, int8 month, int8 year, int8 hours, int8 minutes, int8 seconds)
{
    RTC_DateTypeDef dateStruct;
    dateStruct.WeekDay = RTC_WEEKDAY_MONDAY;
    dateStruct.Month = (uint8)month;
    dateStruct.Date = (uint8)day;
    dateStruct.Year = (uint8)year;

    if (HAL_RTC_SetDate((RTC_HandleTypeDef*)&rtcHandle, &dateStruct, FORMAT_BIN) != HAL_OK)
    {
        return false;
    };

    RTC_TimeTypeDef timeStruct;
    timeStruct.Hours = (uint8)hours;
    timeStruct.Minutes = (uint8)minutes;
    timeStruct.Seconds = (uint8)seconds;
    timeStruct.TimeFormat = RTC_HOURFORMAT_24;
    timeStruct.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    timeStruct.StoreOperation = RTC_STOREOPERATION_SET;

    if (HAL_RTC_SetTime((RTC_HandleTypeDef*)&rtcHandle, &timeStruct, FORMAT_BIN) != HAL_OK)
    {
        return false;
    };

    return true;
}
