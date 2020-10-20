#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "Device.h"
#include "Hardware/Timer.h"
#include "Hardware/Timer.h"
#include "Hardware/VCP.h"
#include "Utils/Math.h"
#include <stdlib.h>
#include <cmath>
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Device::Mode::E Device::currentMode = Device::Mode::Osci;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Device::Init()
{
#ifndef WIN32

    Hardware::Init();

    VCP::Init();

//    Tester::Init();

#endif

  PAUSE_ON_MS(500);


#ifndef WIN32


#endif
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Device::Update()
{
    START_MULTI_MEASUREMENT();

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Device::Mode::E Device::CurrentMode()
{
    return currentMode;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Device::SetMode(Mode::E mode)
{
    currentMode = mode;
    if(currentMode == Mode::Tester)
    {
    }
    else
    {
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Device::InModeTester()
{
    return (currentMode == Device::Mode::Tester);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Device::InModeMultimeter()
{
    return (currentMode == Device::Mode::Multimeter);
}
