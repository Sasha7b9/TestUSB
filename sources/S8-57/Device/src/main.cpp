#include "defines.h"
#include "device.h"
#include "Hardware/Timer.h"


int main(void)
{
    Device::Init();
 
    while (1)
    {
        Device::Update();
    }
}
