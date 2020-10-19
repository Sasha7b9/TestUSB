#include "defines.h"
#include "Timer.h"
#include <climits>
#include "Hardware/HAL/HAL.h"


struct TimerStruct
{
    pFuncVV func;           // ������� �������
    uint    dTms;           // ������ ������������, ��
    uint    timeNextMS;     // ����� ���������� ������������. ���� == 0xffffffff, �� ������ ���������
    bool    repeat;         // ���� true, ����� �����������, ���� �� ����� ������� ������� Timer_Disable()
    uint8   notUsed0;
    uint8   notUsed1;
    uint8   notUsed2;
};



static TimerStruct timers[TypeTimer::Count];
static bool busy = false;



#undef TIME_NEXT
#define TIME_NEXT(type) (timers[type].timeNextMS)



// ������� �����, ������� ����������� � timeStop ��
static void StartTIM(uint timeStop);

static void StopTIM();
// ���������� ����� ������������ ���������� �������, ���� 0, ���� �������� ���
static uint NearestTime();
// ��������� ������� �� ������
static void TuneTIM(::TypeTimer::E type);

void Timer::Init()
{
    for(uint i = 0; i < TypeTimer::Count; i++)
    {
        timers[i].timeNextMS = UINT_MAX; //-V2523
    }

    HAL_TIM3::Init(54000 - 1, 1);
    HAL_TIM3::EnableIRQ(1, 1);

    HAL_TIM2::Init(0, static_cast<uint>(-1));
    HAL_TIM2::Start();
}


void Timer::DeInit()
{
    HAL_TIM2::Stop();
    HAL_TIM2::DeInit();

    HAL_TIM3::DisableIRQ();
    HAL_TIM3::StopIT();
    HAL_TIM3::DeInit();
}


void Timer::SetAndEnable(TypeTimer::E type, pFuncVV func, uint dTms)
{
    Set(type, func, dTms);
    Enable(type);
}


void Timer::Disable(TypeTimer::E type)
{
    timers[type].timeNextMS = UINT_MAX; //-V2523
    timers[type].repeat = false;
}


void Timer::ElapsedCallback()
{
    uint time = TIME_MS;

    if (NearestTime() > time)
    {
        return;
    }

    StopTIM();

    for (uint type = 0; type < TypeTimer::Count; type++)
    {
        if (TIME_NEXT(type) <= time)            // ���� ������ ����� ������������
        {
            TimerStruct *timer = &timers[type];
            if(timer->func)
            {
                timer->func();
            }
            if (timer->repeat)
            {
                do      // ���� ����� ������, ��� ��������� ������ SysTick, ������� ����������� ������������, ����� ��������� ���������,
                {       // � ���� ������� ����������� ������, ��� timer->dTm ��, �� ��� ��� ��������
                    timer->timeNextMS += timer->dTms;
                } while (timer->timeNextMS < TIME_MS);

            }
            else
            {
                timer->timeNextMS = UINT_MAX; //-V2523
            }
        }
    }

    StartTIM(NearestTime());
}


void Timer::Set(TypeTimer::E type, pFuncVV func, uint dTms)
{
    TimerStruct *timer = &timers[type];
    timer->func = func;
    timer->dTms = dTms;
}


void Timer::SetAndStartOnce(TypeTimer::E type, pFuncVV func, uint dTms)
{
    Timer::Set(type, func, dTms);
    StartOnce(type);
}


void Timer::StartOnce(TypeTimer::E type)
{
    timers[type].repeat = false;
    TuneTIM(type);
}


void Timer::Enable(TypeTimer::E type)
{
    timers[type].repeat = true;
    TuneTIM(type);
}


static void TuneTIM(TypeTimer::E type)
{
    TimerStruct *timer = &timers[type];

    uint timeNearest = NearestTime();

    uint timeNext = TIME_MS + timer->dTms;
    timer->timeNextMS = timeNext;

    if(timeNext < timeNearest)      // ���� ������ ������ ��������� ������ ��������
    {
        StartTIM(timeNext);         // �� ������� ������ �� ���� �����
    }
}


static uint NearestTime()
{
    uint time = UINT_MAX; //-V2523

    for(uint type = 0; type < TypeTimer::Count; type++)
    {
        if(TIME_NEXT(type) < time)
        {
            time = TIME_NEXT(type);
        }
    }
    
    return time;
}


static void StartTIM(uint timeStopMS)
{
    StopTIM();

    if(timeStopMS == UINT_MAX) //-V2523
    {
        return;
    }

    uint dT = timeStopMS - TIME_MS;

    HAL_TIM3::StartIT((dT * 2) - 1);             // 10 ������������� 0.1��. �.�. ���� ��� ����� 1��, ����� �������� (100 - 1)
}


static void StopTIM()
{
    HAL_TIM3::StopIT();
}


uint Timer::TimeUS()
{
    return HAL_TIM2::TimeUS();
}


uint Timer::TimeTicks()
{
    return HAL_TIM2::TimeTicks();
}


uint Timer::TimeMS()
{
    return HAL::TimeMS();
}


bool Timer::IsBusy()
{
    return busy;
}
