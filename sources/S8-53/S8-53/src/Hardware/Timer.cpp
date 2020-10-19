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
    //f[type] = eF;
    //reactionTimeMS[type] = timeInMS;
    //currentTimeMS[type] = timeInMS - 1;
    isRun[type] = true;
}
