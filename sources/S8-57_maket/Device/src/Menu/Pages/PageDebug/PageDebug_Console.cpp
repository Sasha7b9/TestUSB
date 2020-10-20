#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "Display/Colors.h"
#include "Display/Painter.h"
#include "Menu/MenuItems.h"
#include "Menu/Pages/Include/PageDebug.h"
#include "Settings/Settings.h"
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const PageBase pageConsole;
extern const PageBase pageRegisters;

const PageBase *PageDebug::PageConsole::pointer = &pageConsole;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_GOVERNOR( gNumStrings,                                                                                                                          //--- ������� - ������� - ����� ����� ---
    "����� �����", "Number strings",
    "",
    "",
    CONSOLE_NUM_STRINGS, 0, 33, pageConsole, FuncActive, FuncChanged, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2( cSizeFont,                                                                                                                          //--- ������� - ������� - ������ ������ ---
    "������ ������", "Size font",
    "",
    "",
    "5", "5",
    "8", "8",
    set.dbg_sizeFont, pageConsole, FuncActive, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2( cModeStop,                                                                                                                          //--- ������� - ������� - ���. �������� ---
    "���. ��������", "Mode stop",
    "������������� ����������� ������������ ������ � ������� ���� ������� �� ������ ����/����",
    "It provides the ability to pause the output to the console by pressing the ����/���� button",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU, ENABLE_EN,
    MODE_PAUSE_CONSOLE, pageConsole, FuncActive, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2( cRegisters_ShowAll,                                                                                                     //--- ������� - ������� - �������� - ���������� ��� ---
    "���������� ���", "Show all",
    "���������� ��� ��������, ���������� � ��������",
    "To show all values transferred in registers",
    "���", "No",
    "��", "Yes",
    DBG_SHOW_ALL, pageRegisters, FuncActive, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsActive_Console_Registers()
{
    return DBG_SHOW_ALL;
}

DEF_CHOICE_2( cRegisters_RD_FL,                                                                                                                //--- ������� - ������� - �������� - RD_FL ---
    "RD_FL", "RD_FL",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU, ENABLE_EN,
    DBG_SHOW_FLAG, pageRegisters, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2( cRegisters_RShiftA,                                                                                                           //--- ������� - ������� - �������� - U ��. 1� ---
    "U ��. 1�", "U shift 1ch",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU, ENABLE_EN,
    set.dbg_showRShift[Chan::A], pageRegisters, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2( cRegisters_RShiftB,                                                                                                           //--- ������� - ������� - �������� - U ��. 2� ---
    "U ��. 2�", "U shift 2ch",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU, ENABLE_EN,
    set.dbg_showRShift[Chan::B], pageRegisters, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2( cRegisters_TrigLev,                                                                                                           //--- ������� - ������� - �������� - U �����. ---
    "U �����.", "U trig.",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU, ENABLE_EN,
    set.dbg_showTrigLev, pageRegisters, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2( cRegisters_RangeA,                                                                                                         //--- ������� - ������� - �������� - �����/��� 1 ---
    "�����/��� 1", "Range 1",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU, ENABLE_EN,
    set.dbg_showRange[Chan::A], pageRegisters, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2( cRegisters_RangeB,                                                                                                         //--- ������� - ������� - �������� - �����/��� 2 ---
    "�����/��� 2", "Range 2",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU, ENABLE_EN,
    set.dbg_showRange[Chan::B], pageRegisters, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2( cRegisters_TrigParam,                                                                                                    //--- ������� - ������� - �������� - �����. �����. ---
    "�����. �����.", "Trig param",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU, ENABLE_EN,
    set.dbg_showTrigParam, pageRegisters, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2( cRegisters_ChanParamA,                                                                                                   //--- ������� - ������� - �������� - �����. ���. 2 ---
    "�����. ���. 1", "Chan 1 param",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU, ENABLE_EN,
    set.dbg_showChanParam[Chan::A], pageRegisters, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2( cRegisters_ChanParamB,                                                                                                   //--- ������� - ������� - �������� - �����. ���. 2 ---
    "�����. ���. 2", "Chan 2 param",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU, ENABLE_EN,
    set.dbg_showChanParam[Chan::B], pageRegisters, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2( cRegisters_TBase,                                                                                                            //--- ������� - ������� - �������� - �����/��� ---
    "�����/���", "TBase",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU, ENABLE_EN,
    set.dbg_showTBase, pageRegisters, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2( cRegisters_TShift,                                                                                                               //--- ������� - ������� - �������� - � ��. ---
    "� ��.", "tShift",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU, ENABLE_EN,
    set.dbg_showTShift, pageRegisters, IsActive_Console_Registers, FuncChangedChoice, FuncDraw
)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_12( pageRegisters, // -V641                                                                                                                   //--- ������� - ������� - �������� ---
    "��������", "REGISTERS",
    "",
    "",
    &cRegisters_ShowAll,        ///< ������� - ������� - �������� - ���������� ���
    &cRegisters_RD_FL,          ///< ������� - ������� - �������� - RD_FL
    &cRegisters_RShiftA,        ///< ������� - ������� - �������� - U ��. 1�
    &cRegisters_RShiftB,        ///< ������� - ������� - �������� - U ��. 2�
    &cRegisters_TrigLev,        ///< ������� - ������� - �������� - U �����
    &cRegisters_RangeA,         ///< ������� - ������� - �������� - �����/��� 1
    &cRegisters_RangeB,         ///< ������� - ������� - �������� - �����/��� 2
    &cRegisters_TrigParam,      ///< ������� - ������� - �������� - �����. �����.
    &cRegisters_ChanParamA,     ///< ������� - ������� - �������� - �����. ���. 1
    &cRegisters_ChanParamB,     ///< ������� - ������� - �������� - �����. ���. 2
    &cRegisters_TBase,          ///< ������� - ������� - �������� - �����/���
    &cRegisters_TShift,         ///< ������� - ������� - �������� - � ��.
    Page::Name::Debug_Console_Registers, &pageConsole, FuncActive, EmptyPressPage, FuncDrawPage, FuncRegSetPage
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void Draw_Console_SizeSettings(int x, int y)
{
    String("����.�����. %d", sizeof(Settings)).Draw(x + 6, y + 13, Color::BACK);
}

DEF_BUTTON( bSizeSettings,                                                                                                                      //--- ������� - ������� - ������ �������� ---
    "", "",
    "���������� ������� ������ ��������� ��� ���������� ��������",
    "Displays the current size of the structure to save settings",
    pageConsole, FuncActive, FuncPress, Draw_Console_SizeSettings
)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_5( pageConsole, // -V641 // -V1027                                                                                                                       //--- ������� - ������� ---
    "�������", "CONSOLE",
    "",
    "",
    &gNumStrings,       ///< ������� - ������� - ����� �����
    &cSizeFont,         ///< ������� - ������� - ������ ������
    &cModeStop,         ///< ������� - ������� - ���. ��������
    &pageRegisters,     ///< ������� - ������� - ��������
    &bSizeSettings,     ///< ������� - ������� - ������ ��������
    Page::Name::Debug_Console, PageDebug::pointer, FuncActive, EmptyPressPage, FuncDrawPage, FuncRegSetPage
)

