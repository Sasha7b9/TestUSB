#ifndef WIN32
#include "defines.h"
#include "Device.h"
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
