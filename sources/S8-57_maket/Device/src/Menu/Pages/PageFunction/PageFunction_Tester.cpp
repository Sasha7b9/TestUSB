
#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "Tester/Tester.h"
#include "Settings/Settings.h"
#include "Menu/Pages/Include/PageFunction.h"
#include "Device.h"
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const PageBase pageTester;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void OnChanged_Polarity(bool)
{
    Tester::LoadPolarity();
}


DEF_CHOICE_2(   cPolarity,                                                                                                                            //--- ������-��������� - ���������� ---
    "����������", "Polarity",
    "���������� �������������� �����������",
    "Polarity of the test exposure",
    "+", "+",
    "-", "-",
    TESTER_POLARITY, pageTester, FuncActive, OnChanged_Polarity, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(   cControl,                                                                                                                             //--- ������-��������� - ���������� ---
    "����������", "Control",
    "��� �������������� �����������",
    "Type of test exposure",
    "����������", "Voltage",
    "���", "Current",
    TESTER_CONTROL, pageTester, FuncActive, PageFunction::PageTester::OnChanged_Control, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_Step(bool)
{
    Tester::LoadStep();
}

DEF_CHOICE_2(   cStepU,                                                                                                                                      //--- ������-��������� - ��� ---
    "���", "Step",
    "��� �������������� ����������",
    "Test voltage step",
    "100 ��", "100 mV",
    "500 ��", "500 mV",
    TESTER_STEP_U, pageTester, FuncActive, OnChanged_Step, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(   cStepI,
    "���", "Step",
    "��� ��������������� ����",
    "Step test current",
    "4 ��",  "4 mA",
    "20 ��", "20 mA",
    TESTER_STEP_I, pageTester, FuncActive, OnChanged_Step, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(   cViewMode,
    "���������", "Paint",
    "", "",
    "�����", "Lines",
    "�����", "Points",
    TESTER_VIEW_MODE, pageTester, FuncActive, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_5(   cSmoothing,
    "�����������", "Smoothing",
    "", "",
    "1", "1",
    "2", "2",
    "3", "3",
    "4", "4",
    "5", "5",
    TESTER_NUM_SMOOTH, pageTester, FuncActive, FuncChangedChoice, FuncDraw
)


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const PageBase *PageFunction::PageTester::pointer = &pageTester;

void PageFunction::PageTester::Init()
{
    OnChanged_Control(true);
}

static void OnEnterExit_Tester(bool enter)
{
    Device::SetMode(enter ? Device::Mode::Tester : Device::Mode::Osci);
}


DEF_PAGE_4( pageTester, // -V641
    "������", "TESTER",
    "", "",
    &cControl,
    &cStepU,
    &cPolarity,
    &cViewMode,
    //&cSmoothing,
    Page::Name::Function_Tester, PageFunction::pointer, FuncActive, OnEnterExit_Tester, FuncDrawPage, FuncRegSetPage
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void PageFunction::PageTester::OnChanged_Control(bool)
{
    PageBase *page = (PageBase *)&pageTester;

    Control **items = (Control **)page->items;

    if (TESTER_CONTROL_IS_U)
    {
        items[1] = (Control *)&cStepU; //-V641
    }
    else
    {
        items[1] = (Control *)&cStepI; //-V641
    }

    Tester::LoadStep();
}
