#include "defines.h"
#include "main.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Hardware/Hardware.h"
#include "VCP/VCP.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TICS ((gTimerTics - time) / 120.0f)

extern void main3();    

//----------------------------------------------------------------------------------------------------------------------------------------------------
int main(void)
{
    Hardware::Init();
    VCP::Init();
//    Timer::PauseOnTime(250);

    while(1)
    {
    }
}
