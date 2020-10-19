#include "Clock.h"
#include "Hardware/Hardware.h"
#include "Log.h"
#include <stm32f2xx_hal.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Clock::Init(void)
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
PackedTime Clock::GetPackedTime(void)
{
    PackedTime time;

    return time;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool Clock::SetTimeAndData(int8 day, int8 month, int8 year, int8 hours, int8 minutes, int8 seconds)
{
    return true;
}
