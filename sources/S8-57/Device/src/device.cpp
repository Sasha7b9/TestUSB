#include "defines.h"
#include "device.h"
#include "Hardware/Timer.h"
#include "Hardware/VCP.h"
#include "Hardware/HAL/HAL.h"
#include <cstdlib>


// Настроить устройство в соответствии с установленным режимом
static void SetCurrentMode();

static Device::Mode::E currentMode = Device::Mode::Osci;

void Device::Init()
{
    HAL::Init();

    Timer::Init();

    PAUSE_ON_MS(500);

    VCP::Init();

    SetCurrentMode();
}


void SetCurrentMode()
{
}


void Device::Update()
{
    Timer::StartMultiMeasurement();
}


Device::Mode::E Device::CurrentMode()
{
    return currentMode;
}


void Device::SetMode(Mode::E mode)
{
}


bool Device::InModeTester()
{
    return (CurrentMode() == Device::Mode::Tester);
}


bool Device::InModeMultimeter()
{
    return (CurrentMode() == Device::Mode::Multimeter);
}


bool Device::InModeOsci()
{
    return (CurrentMode() == Device::Mode::Osci);
}


bool Device::InModeRecorder()
{
    return (CurrentMode() == Device::Mode::Recorder);
}


void Device::Reset()
{
    HAL_NVIC::SystemReset();
}
