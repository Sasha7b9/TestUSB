#include "stdafx.h"
#ifndef WIN32
#include <stm32f4xx.h>
#include "defines.h"
#include "CPU.h"
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPU::Init()
{
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CPU::RTC_::SetCorrection(int8)
{
}
