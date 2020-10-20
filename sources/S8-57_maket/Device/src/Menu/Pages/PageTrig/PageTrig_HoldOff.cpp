#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Menu/Pages/Include/PageTrig.h"
#include "Menu/MenuItems.h"
#include "Settings/Settings.h"
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const PageBase pageHoldOff;
const PageBase *PageTrig::PageHoldOff::pointer = &pageHoldOff;


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_HoldOff(bool)
{
    FPGA::LoadHoldfOff();
}

DEF_CHOICE_2( cHoldOff,                                                                                                                               //--- ����� - ��������� - ��������� ---
    "���������", "Hold Off",
    "��������/��������� ����� �������� �������������",
    "Enable / disable sync delay mode",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    TRIG_HOLDOFF_ENABLED, pageHoldOff, FuncActive, OnChanged_HoldOff, FuncDraw
)


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_Duration()
{
    FPGA::LoadHoldfOff();
}

DEF_GOVERNOR( gDuration,                                                                                                                           //--- ����� - ��������� - ������������ ---
    "������������", "Duration",
    "����� ������������ ���������� ����� ���������� �������������",
    "Sets the duration of the gap between synchronization pulses",
    TRIG_HOLDOFF, 10, 15000000, pageHoldOff, FuncActive, OnChanged_Duration, FuncBeforeDraw
)


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_PAGE_2( pageHoldOff, // -V641 // -V1027                                                                                                                       //--- ����� - ��������� ---
    "���������", "HOLD OFF",
    "",
    "",
    &cHoldOff,          ///< ����� - ��������� - ���������
    &gDuration,         ///< ����� - ��������� - ������������
    Page::Name::Trig_HoldOff, PageTrig::pointer, FuncActive, EmptyPressPage, FuncDrawPage, FuncRegSetPage
)

