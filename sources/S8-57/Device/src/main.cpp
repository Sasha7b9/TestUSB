#include "defines.h"
#include "device.h"


int main(void)
{
    Device::Init();
 
    while (1)
    {
        Device::Update();
    }
}
