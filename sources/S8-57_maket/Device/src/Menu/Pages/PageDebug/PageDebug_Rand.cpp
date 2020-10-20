#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Menu/MenuItems.h"
#include "Menu/Pages/Include/PageDebug.h"
#include "Settings/Settings.h"
#include "Settings/SettingsNRST.h"
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const PageBase pageRand;

const PageBase *PageDebug::PageRand::pointer = &pageRand;


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(gRand_NumAverage,                                                                           //--- ������� - ����-��� - ������. ---
    "������.", "Average",
    "",
    "",
    NRST_NUM_AVE_FOR_RAND, 1, 32, pageRand, FuncActive, FuncChanged, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(gRand_NumSmooth,                                                                        //--- ������� - ����-��� - ����������� ---
    "�����������", "Smoothing",
    "",
    "",
    NRST_NUM_SMOOTH_FOR_RAND, 1, 10, pageRand, FuncActive, FuncChanged, FuncBeforeDraw
)

static void OnChanged_Rand_NumMeasures()
{
    FPGA::SetNumberMeasuresForGates(NUM_MEASURES_FOR_GATES);
}

DEF_GOVERNOR(gRand_NumMeasures,                                                                     //--- ������� - ����-��� - ���-�/������ ---
    "���-�/������", "Samples/gates",
    "",
    "",
    NUM_MEASURES_FOR_GATES, 1, 2500, pageRand, FuncActive, OnChanged_Rand_NumMeasures, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(cRand_ShowInfo,                                                                          //--- ������� - ����-��� - ���������� ---
    "����������", "Information",
    "���������� ���������� � ������� �������������",
    "To show information on randomizer gate",
    "�� ����������", "Hide",
    "����������", "Show",
    SHOW_RAND_INFO, pageRand, FuncActive, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(gRand_ShowStat,                                                                          //--- ������� - ����-��� - ���������� ---
    "����������", "Statistics",
    "���������� ������ ����������",
    "Statistics show schedule",
    "�� ����������", "Hide",
    "����������", "Show",
    SHOW_RAND_STAT, pageRand, FuncActive, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_Rand_TimeCompensation()
{
    FPGA::SetDeltaTShift(TIME_COMPENSATION);
}

DEF_GOVERNOR(gRand_TimeCompensation,                                                            //--- ������� - ����-��� - ����������� �������� ---
    "����������� ��������", "Compenstaion time",
    "���������� ����������� �������� ��� 40 ��",
    "",
    TIME_COMPENSATION, 0, 510, pageRand, FuncActive, OnChanged_Rand_TimeCompensation, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_Rand_AddTimeShift()
{
    FPGA::SetTShift(SET_TSHIFT);
}

static int16 addShift = 0;

DEF_GOVERNOR(gRand_AddTimeShift,                                                                        //--- ������� - ����-��� - �������� ---
    "��� ��������", "Add shift",
    "���������� ������� ��� �������� tShift",
    "",
    addShift, -100, 100, pageRand, FuncActive, OnChanged_Rand_AddTimeShift, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_Rand_Pretriggered()
{
    FPGA::hardware.LoadTShift();
}

DEF_GOVERNOR(gRand_Pretriggered,                                                                      //--- ������� - ����-��� - ���������� ---
    "����������", "Pretiggered",
    "�������� �����������, ������� ������� � ������������",
    "",
    PRETRIGGERED, 0, 30000, pageRand, FuncActive, OnChanged_Rand_Pretriggered, FuncBeforeDraw
)


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_16( gGateMax,                                                                                                                           //--- ������� - ����-��� - ������ ���� ---
    "������ ����", "Gates up",
    "������������� ����������� ���������� �� ��������� ����� ��� �������������",
    "",
    "0",  "0",
    "10", "10",
    "20", "20",
    "30", "30",
    "40", "40",
    "50", "50",
    "60", "60",
    "70", "70",
    "80", "80",
    "90", "90",
    "100", "100",
    "110", "110",
    "120", "120",
    "130", "130",
    "140", "140",
    "150", "150",
    set.dbg_enum_gate_max, pageRand, FuncActive, FuncChangedChoice, FuncDraw
)


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_16( gGateMin,                                                                                                                            //--- ������� - ����-��� - ������ ��� ---
    "������ ���", "Gates down",
    "������������� ����������� ���������� �� �������� ����� ��� �������������",
    "",
    "0", "0",
    "10", "10",
    "20", "20",
    "30", "30",
    "40", "40",
    "50", "50",
    "60", "60",
    "70", "70",
    "80", "80",
    "90", "90",
    "100", "100",
    "110", "110",
    "120", "120",
    "130", "130",
    "140", "140",
    "150", "150",
    set.dbg_enum_gate_min, pageRand, FuncActive, FuncChangedChoice, FuncDraw
)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_2( pageRand, // -V641 // -V1027                                                                                                                         //--- ������� - ����-��� ---
    "����-���", "RANDOMIZER",
    "",
    "",
    &gGateMin,                      ///< ������� - ����-��� - ������ ���
    &gGateMax,                      ///< ������� - ����-��� - ������ ����

    //&gRand_NumAverage,          ///< ������� - ����-��� - ������.
    //&gRand_NumSmooth,           ///< ������� - ����-��� - �����������
    //&gRand_NumMeasures,         ///< ������� - ����-��� - ���-�/������
    //&cRand_ShowInfo,            ///< ������� - ����-��� - ����������
    //&gRand_ShowStat,            ///< ������� - ����-��� - ����������
    //&gRand_TimeCompensation,    ///< ������� - ����-��� - ����������� ��������
    //&gRand_AddTimeShift,        ///< ������� - ����-��� - ��������
    //&gRand_Pretriggered,        ///< ������� - �A��-��� - ����������
    Page::Name::Debug_Rand, PageDebug::pointer, FuncActive, EmptyPressPage, FuncDrawPage, FuncRegSetPage
)
