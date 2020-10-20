#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "main.h"
#include "Device.h"
#include "Hardware/Timer.h"
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    Device::Init();

    while (1)
    {
        Device::Update();
    }
}
