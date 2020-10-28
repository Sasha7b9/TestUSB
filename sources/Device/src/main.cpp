#ifndef WIN32
#include "defines.h"
#include "Hardware/Hardware.h"
#include "Hardware/VCP.h"
#endif

uint8 newbuff[100] = {0};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    Hardware::Init();

    VCP::Init();

    while(1)
    {
        if(VCP::BuffRx() != 0)
        {
            VCP::SendStringSynch((char*) VCP::BuffRx());
        }
    }
}
