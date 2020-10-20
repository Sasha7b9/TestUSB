#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "Settings/Settings.h"
#include "Menu/Pages/Include/Definition.h"
#include "Menu/Pages/Include/PageFunction.h"
#include "Device.h"
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const PageBase pageMultimeter;

const PageBase *PageFunction::PageMultimeter::pointer = &pageMultimeter;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool FuncActive_RangesVoltageDC()
{
    return MULTI_MEASURE == Multimeter::Measure::VoltageDC;
}

static void OnChange_VoltageDC(bool active)
{
    if(active)
    {
        Multimeter::ChangeMode();
    }
}

DEF_CHOICE_3( cRangesVoltageDC,                                                                                                                                 //--- ���������� - ������ ---
    "������", "Range",
    "�������� ���������", "�easurement range",
    "2 �",   "2 V",
    "20 �",  "20 V",
    "500 �", "500 V",
    MULTI_RANGE_DC, pageMultimeter, FuncActive_RangesVoltageDC, OnChange_VoltageDC, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static bool FuncActive_RnagesVoltageAC()
{
    return MULTI_MEASURE == Multimeter::Measure::VoltageAC;
}

static void OnChange_VoltageAC(bool active)
{
    if(active)
    {
        Multimeter::ChangeMode();
    }
}

DEF_CHOICE_3( cRangesVoltageAC,                                                                                                                                 //--- ���������� - ������ ---
    "������", "Range",
    "�������� ���������", "Measurement range",
    "2 �", "2 V",
    "20 �", "20 V",
    "400 �", "400 V",
    MULTI_RANGE_AC, pageMultimeter, FuncActive_RnagesVoltageAC, OnChange_VoltageAC, FuncDraw
)


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static uint8 rangeCurrent;

DEF_CHOICE_2( cRangesCurrent,
    "������", "Range",
    "�������� ���������", "Measurement range",
    "2�", "2�",
    "2�", "2�",
    rangeCurrent, pageMultimeter, EmptyFuncBfV, FuncChangedChoice, FuncDraw
)


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static bool FuncActive_RangesReistance()
{
    return MULTI_MEASURE == Multimeter::Measure::Resistance;
}

static void OnChange_Resistance(bool active)
{
    if(active)
    {
        Multimeter::ChangeMode();
    }
}

DEF_CHOICE_4(   cRangesResistance,                                                                                                                              //--- ���������� - ������ ---
    "������", "Range",
    "�������� ���������", "Measurement range",
    "2 ���", "2 kOhm",
    "20 ���", "20 kOhm",
    "200 ���", "200 kOhm",
    "10 ���", "10 MOhm",
    MULTI_RANGE_RESISTANCE, pageMultimeter, FuncActive_RangesReistance, OnChange_Resistance, FuncDraw
)

DEF_CHOICE_7(   cMode,
    "�����", "Mode"
    ,
    "����� ���� ����������\n"
    "U= - ���������� ����������\n"
    "U~ - ���������� ����������\n"
    "I= - ���������� ���\n"
    "I~ - ���������� ���\n"
    "R - ������������� ����������� ����\n"
    "VD - ���� ������\n"
    "BELL - ���������"
    ,
    "Selecting the type of measurement\n"
    "U= - DC voltage\n"
    "U~ - AC voltage\n"
    "I= - DC current\n"
    "I~ - AC current\n"
    "R - DC resistance\n"
    "VD - diodes test\n"
    "BELL - bell"
    ,
    "U=",   "U=",
    "U~",   "U~",
    "I=",   "I=",
    "I~",   "I~",
    "R",    "R",
    "VD",   "VD",
    "BELL", "BELL",
    MULTI_MEASURE, pageMultimeter, FuncActive, PageFunction::PageMultimeter::OnChanged_Mode, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_AVP(bool)
{
    Multimeter::ChangeAVP();
}

DEF_CHOICE_2 (cAVP,
    "���", "ASL",
    "��������� �������",
    "Auto-select limit",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU, ENABLE_EN,
    MULTI_AVP, pageMultimeter, FuncActive, OnChanged_AVP, FuncDraw
)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PageFunction::PageMultimeter::Init()
{
    OnChanged_Mode(true);
}

static void OnPress_Page(bool enter)
{
    Device::SetMode(enter ? Device::Mode::Multimeter : Device::Mode::Osci);
}

DEF_PAGE_3( pageMultimeter, // -V641
    "����������", "MULTIMETER",
    "���������� �������� � ������ �����������",
    "Instrument control in multimeter mode",
    &cMode,
    &cRangesVoltageDC,
    &cAVP,
    Page::Name::Function_Multimeter, PageFunction::pointer, FuncActive, OnPress_Page, FuncDrawPage, FuncRegSetPage
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void PageFunction::PageMultimeter::OnChanged_Mode(bool)
{
    PageBase *page = (PageBase *)&pageMultimeter;

    Control **items = (Control **)page->items;

    if (MULTI_MEASURE == Multimeter::Measure::VoltageDC)
    {
        items[1] = (Control *)&cRangesVoltageDC; //-V641
    }
    else if (MULTI_MEASURE == Multimeter::Measure::VoltageAC)
    {
        items[1] = (Control *)&cRangesVoltageAC; //-V641
    }
    else if (MULTI_MEASURE == Multimeter::Measure::CurrentDC || MULTI_MEASURE == Multimeter::Measure::CurrentAC)
    {
        items[1] = (Control *)&cRangesCurrent;  // -V641
    }
    else if (MULTI_MEASURE == Multimeter::Measure::Resistance)
    {
        items[1] = (Control *)&cRangesResistance; //-V641
    }
    else
    {
        LOG_ERROR("");
    }

    Multimeter::ChangeMode();
}
