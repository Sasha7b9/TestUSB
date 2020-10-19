#include "defines.h"
#include "Timer.h"
#include "Log.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void (*f[TypeTimerSize])() = {0};
static int reactionTimeMS[TypeTimerSize] = {0};
static int currentTimeMS[TypeTimerSize] = {0};
static bool isRun[TypeTimerSize] = {false};


//----------------------------------------------------------------------------------------------------------------------------------------------------
void Timer::Enable(TypeTimer type, int timeInMS, void(*eF)())
{
    f[type] = eF;
    reactionTimeMS[type] = timeInMS;
    currentTimeMS[type] = timeInMS - 1;
    isRun[type] = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Timer::Disable(TypeTimer type)
{
    isRun[type] = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Timer_Pause(TypeTimer type)
{
    isRun[type] = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Timer_Continue(TypeTimer type)
{
    isRun[type] = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
bool Timer_IsRun(TypeTimer type)
{
    return isRun[type];
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Timer_Update10ms(void)
{
    for(int num = 0; num < TypeTimerSize; num++)
    {
        if(isRun[num])
        {
            currentTimeMS[num] -= 10;
            if(currentTimeMS[num] < 0)
            {
                f[num]();
                currentTimeMS[num] = reactionTimeMS[num] - 1;
            }

        }
    }
}
