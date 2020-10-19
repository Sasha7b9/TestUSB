#include "defines.h"
#include "device.h"
#include "Hardware/Timer.h"


int main(void)
{
    Device::Init();

    Timer::PauseOnTime(1);

//    AT25160N::Init();

//    AT25160N::Test();
    
//    Test::Run();
  
    while (1)
    {
        Device::Update();
    }
}
