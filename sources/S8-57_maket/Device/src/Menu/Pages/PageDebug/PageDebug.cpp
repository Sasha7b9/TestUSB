#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "Menu/Pages/Include/PageDebug.h"
#include "Data/Reader.h"
#include "Display/Grid.h"
#include "Display/Symbols.h"
#include "Display/Painter.h"
#include "FlashDrive/FlashDrive.h"
#include "FPGA/FPGA.h"
#include "Hardware/EEPROM.h"
#include "Hardware/Sound.h"
#include "Menu/Menu.h"
#include "Menu/Pages/Include/Definition.h"
#include "Utils/CommonFunctions.h"
#include "Utils/Dictionary.h"
#include "Utils/Math.h"
#include "Utils/StringUtils.h"
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const PageBase mainPage;
extern const PageBase pageDebug;
extern const PageBase ppADC;
extern const PageBase pppADC_Balance;
extern const PageBase pppADC_Stretch;
extern const PageBase pppADC_Shift;
extern const PageBase ppChannels;
extern const PageBase ppSettings;
extern const PageBase ppSerialNumber;

const PageBase *PageDebug::pointer = &pageDebug;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// � ���� ��������� ����� ��������� ������ ��������� ������ ��� �������� �������� ppSerialNumer
typedef struct
{
    int number;     ///< ��������������, ���������� �����.
    int year;       ///< ��������������, ���.
    int curDigt;    ///< ��������������, ������� (0) ��� ����� (1) ��������� ����� ���������.
} StructForSN;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static int16 shiftADCA;
static int16 shiftADCB;

static void Draw_ADC_Balance_Mode(int, int)
{
    int8 shift[2][3] =
    {
        {0, SET_BALANCE_ADC_A, (int8)NRST_BALANCE_ADC_A},
        {0, SET_BALANCE_ADC_B, (int8)NRST_BALANCE_ADC_B}
    };

    shiftADCA = shift[Chan::A][NRST_BALANCE_ADC_TYPE];
    shiftADCB = shift[Chan::B][NRST_BALANCE_ADC_TYPE];
}

static void OnChanged_ADC_Balance_Mode(bool)
{
    Draw_ADC_Balance_Mode(0, 0);
}

DEF_CHOICE_3(       cADC_Balance_Mode,                                                                        //--- ������� - ��� - ������ - ����� ---
    "�����", "Mode",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    "��������", "Real",
    "������",   "Manual",
    NRST_BALANCE_ADC_TYPE, pppADC_Balance, FuncActive, OnChanged_ADC_Balance_Mode, Draw_ADC_Balance_Mode
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsActive_ADC_Balance_ShiftAB()
{
    return NRST_BALANCE_ADC_TYPE_IS_HAND;
}

static void OnChanged_ADC_Balance_ShiftA()
{
    NRST_BALANCE_ADC_A = shiftADCA;
}

DEF_GOVERNOR(       gADC_Balance_ShiftA,                                                                 //--- ������� - ��� - ������ - �������� 1 ---
    "�������� 1", "Offset 1",
    "",
    "",
    shiftADCA, -125, 125, pppADC_Balance, IsActive_ADC_Balance_ShiftAB, OnChanged_ADC_Balance_ShiftA, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_ADC_Balance_ShiftB()
{
    NRST_BALANCE_ADC_B = shiftADCB;
}

DEF_GOVERNOR(       gADC_Balance_ShiftB,                                                                 //--- ������� - ��� - ������ - �������� 2 ---
    "�������� 2", "Offset 2",
    "",
    "",
    shiftADCB, -125, 125, pppADC_Balance, IsActive_ADC_Balance_ShiftAB, OnChanged_ADC_Balance_ShiftB, FuncBeforeDraw
)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_3( pppADC_Balance, // -V641                                                                                                                         //--- ������� - ��� - ������ ---
    "������", "BALANCE",
    "",
    "",
    &cADC_Balance_Mode,      // ������� - ��� - ������ - �����
    &gADC_Balance_ShiftA,    // ������� - ��� - ������ - �������� 1
    &gADC_Balance_ShiftB,    // ������� - ��� - ������ - �������� 2
    Page::Name::Debug_ADC_Balance, &ppADC, FuncActive, EmptyPressPage, FuncDrawPage, FuncRegSetPage
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static int16 stretchA;
static int16 stretchB;

void PageDebug::OnChanged_ADC_Stretch_Mode(bool)
{
    if (NRST_STRETCH_ADC_TYPE_IS_DISABLE)
    {
        stretchA = NRST_STRETCH_ADC_A(StretchADC::Disable) = 0;
        stretchB = NRST_STRETCH_ADC_B(StretchADC::Disable) = 0;
    }
    else
    {
        stretchA = NRST_STRETCH_ADC_A(NRST_STRETCH_ADC_TYPE);
        stretchB = NRST_STRETCH_ADC_B(NRST_STRETCH_ADC_TYPE);
    }
}

DEF_CHOICE_3(       cADC_Stretch_Mode,                                                                      //--- ������� - ��� - �������� - ����� ---
    "�����", "Mode",
    "",
    "",
    DISABLE_RU, DISABLE_EN,
    "��������", "Real",
    "������",   "Manual",
    NRST_STRETCH_ADC_TYPE, pppADC_Stretch, FuncActive, PageDebug::OnChanged_ADC_Stretch_Mode, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsActive_ADC_StretchAB()
{
    return NRST_STRETCH_ADC_TYPE_IS_HAND;
}

static void OnChanged_ADC_Stretch_A()
{
    NRST_STRETCH_ADC_A(NRST_STRETCH_ADC_TYPE) = stretchA;
}

DEF_GOVERNOR(       gADC_Stretch_A,                                                                   //--- ������� - ��� - �������� - �������� 1� ---
    "�������� 1�", "Stretch 1ch",
    "����� ������ �������� ������� ������.\n1 ������� = 0.0001",
    "Sets the manual stretching of the first channel.\n1 = 0.0001",
    stretchA, -10000, 10000, pppADC_Stretch, IsActive_ADC_StretchAB, OnChanged_ADC_Stretch_A, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_ADC_Stretch_B()
{
    NRST_STRETCH_ADC_B(NRST_STRETCH_ADC_TYPE) = stretchB;
}

DEF_GOVERNOR(       gADC_Stretch_B,                                                                   //--- ������� - ��� - �������� - �������� 2� ---
    "�������� 2�", "Stretch 2ch",
    "����� ������ �������� ������� ������.\n1 ������� = 0.0001",
    "Sets the manual stretching of the second channel.\n1 = 0.0001",
    stretchB, -10000, 10000, pppADC_Stretch, IsActive_ADC_StretchAB, OnChanged_ADC_Stretch_B, FuncBeforeDraw
)

/*
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       gADC_Stretch_Ak20mV,                                                               //--- ������� - ��� - �������� - 20��/1� 1� ---
    "20��/1� 1�", "20mV/1V 1k",
    "",
    "",
    NRST_ADD_STRETCH_20mV_A, -10000, 10000, pppADC_Stretch, FuncActive, FuncChanged, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       gADC_Stretch_Ak50mV,                                                                  //--- ������� - ��� - �������� - 50�� 1� ---
    "50�� 1�", "50mV 1k",
    "",
    "",
    NRST_ADD_STRETCH_50mV_A, -10000, 10000, pppADC_Stretch, FuncActive, FuncChanged, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       gADC_Stretch_Ak100mV,                                                             //--- ������� - ��� - �������� - 100��/5� 1� ---
    "100��/5� 1�", "100mV/5V 1ch",
    "",
    "",
    NRST_ADD_STRETCH_100mV_A, -10000, 10000, pppADC_Stretch, FuncActive, FuncChanged, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       gADC_Stretch_Ak2V,                                                                      //--- ������� - ��� - �������� - 2� 1� ---
    "2� 1�", "2V 1ch",
    "",
    "",
    NRST_ADD_STRETCH_2V_A, -10000, 10000, pppADC_Stretch, FuncActive, FuncChanged, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       gADC_Stretch_Bk20mV,                                                               //--- ������� - ��� - �������� - 20��/1� 2� ---
    "20��/1� 2�", "20mV/1V 2k",
    "",
    "",
    NRST_ADD_STRETCH_20mV_B, -10000, 10000, pppADC_Stretch, FuncActive, FuncChanged, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       gADC_Stretch_Bk50mV,                                                         //--- ������� - ��� - �������� - ��� ���� 50�� 2� ---
    "50�� 2�", "50mV 2k",
    "",
    "",
    NRST_ADD_STRETCH_50mV_B, -10000, 10000, pppADC_Stretch, FuncActive, FuncChanged, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       gADC_Stretch_Bk100mV,                                                             //--- ������� - ��� - �������� - 100��/5� 2� ---
    "100��/5� 2�", "100mV/5V 2k",
    "",
    "",
    NRST_ADD_STRETCH_100mV_B, -10000, 10000, pppADC_Stretch, FuncActive, FuncChanged, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       gADC_Stretch_Bk2V,                                                                      //--- ������� - ��� - �������� - 2� 2� ---
    "2� 2�", "2V 2ch",
    "",
    "",
    NRST_ADD_STRETCH_2V_B, -10000, 10000, pppADC_Stretch, FuncActive, FuncChanged, FuncBeforeDraw
)
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//static const ChoiceBase emptyChoice = {Control::Type::Choice, 0, false, Page::Name::NoPage, 0, FuncActive, {}, 0, 0, 0, 0};

DEF_PAGE_3( pppADC_Stretch, // -V641                                                                                                                       //--- ������� - ��� - �������� ---
    "��������", "STRETCH",
    "������������� ����� � �������� �������� (��� ������� ������)",
    "Sets mode and the value of stretching (manual mode)",
    &cADC_Stretch_Mode,      // ������� - ��� - �������� - �����
    &gADC_Stretch_A,         // ������� - ��� - �������� - �������� 1�
    &gADC_Stretch_B,
    Page::Name::Debug_ADC_Stretch, &ppADC, FuncActive, EmptyPressPage, FuncDrawPage, FuncRegSetPage
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_ADC_Shift_Reset()
{
    for (int ch = 0; ch < 2; ch++)
    {
        for (int range = 0; range < 3; range++)
        {
            RSHIFT_ADD_STABLE(ch, range) = 0;
        }
    }
    FPGA::SetRShift(Chan::A, SET_RSHIFT_A);
    FPGA::SetRShift(Chan::B, SET_RSHIFT_B);
}

DEF_BUTTON(         bADC_Shift_Reset,                                                                        //-- ������� - ��� - ��� ���� - ����� ---
    "�����", "Reset",
    "",
    "",
    pppADC_Shift, FuncActive, OnPress_ADC_Shift_Reset, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_ADC_Shift_A()
{
    FPGA::SetRShift(Chan::A, SET_RSHIFT_A);
}

DEF_GOVERNOR(       gADC_Shift_A2mV,                                                               //--- ������� - ��� - ��� ���� - �� 1� 2�� ���� ---
    "�� 1� 2�� ����", "Shift 1ch 2mV DC",
    "",
    "",
    RSHIFT_ADD_STABLE_A(Range::_2mV), -100, 100, pppADC_Shift, FuncActive, OnChanged_ADC_Shift_A, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_ADC_Shift_B()
{
    FPGA::SetRShift(Chan::B, SET_RSHIFT_B);
}

DEF_GOVERNOR(       gADC_Shift_B2mV,                                                               //--- ������� - ��� - ��� ���� - �� 2� 2�� ���� ---
    "�� 2� 2�� ����", "Shift 2ch 2mV DC",
    "",
    "",
    RSHIFT_ADD_STABLE_B(Range::_2mV), -100, 100, pppADC_Shift, FuncActive, OnChanged_ADC_Shift_B, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       gADC_Shift_A5mV,                                                               //--- ������� - ��� - ��� ���� - �� 1� 5�� ���� ---
    "�� 1� 5�� ����", "Shift 1ch 5mV DC",
    "",
    "",
    RSHIFT_ADD_STABLE_A(Range::_5mV), -100, 100, pppADC_Shift, FuncActive, OnChanged_ADC_Shift_A, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       gADC_Shift_B5mV,                                                               //--- ������� - ��� - ��� ���� - �� 2� 5�� ���� ---
    "�� 2� 5�� ����", "Shift 2ch 5mV DC",
    "",
    "",
    RSHIFT_ADD_STABLE_B(Range::_5mV), -100, 100, pppADC_Shift, FuncActive, OnChanged_ADC_Shift_B, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR(       gADC_Shift_A10mV,                                                             //--- ������� - ��� - ��� ���� - �� 1� 10�� ���� ---
    "�� 1� 10�� ����", "Shift 1ch 10mV DC",
    "",
    "",
    RSHIFT_ADD_STABLE_A(Range::_10mV), -100, 100, pppADC_Shift, FuncActive, OnChanged_ADC_Shift_A, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_GOVERNOR( gADC_Shift_B10mV,                                                                                                          //--- ������� - ��� - ��� ���� - �� 2� 10�� ���� ---
    "�� 2� 10�� ����", "Shift 2ch 10mV DC",
    "",
    "",
    RSHIFT_ADD_STABLE_B(Range::_10mV), -100, 100, pppADC_Shift, FuncActive, OnChanged_ADC_Shift_B, FuncBeforeDraw
)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_7( pppADC_Shift, // -V641  // -V1027                                                                                                              //--- ������� - ��� - ��� ���� ---
    "��� ����", "ADD RSHFIT",
    "",
    "",
    &bADC_Shift_Reset,   // ������� - ��� - ��� ���� - �����
    &gADC_Shift_A2mV,    // ������� - ��� - ��� ���� - �� 1� 2�� ����
    &gADC_Shift_B2mV,    // ������� - ��� - ��� ���� - �� 2� 2�� ����
    &gADC_Shift_A5mV,    // ������� - ��� - ��� ���� - �� 1� 5�� ����
    &gADC_Shift_B5mV,    // ������� - ��� - ��� ���� - �� 2� 5�� ����
    &gADC_Shift_A10mV,   // ������� - ��� - ��� ���� - �� 1� 10�� ����
    &gADC_Shift_B10mV,   // ������� - ��� - ��� ���� - �� 2� 10�� ����
    Page::Name::Debug_ADC_Shift, &ppADC, FuncActive, EmptyPressPage, FuncDrawPage, FuncRegSetPage
)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_3( ppADC, // -V641                                                                                                                                           //--- ������� - ��� ---
    "���", "ADC",
    "",
    "",
    &pppADC_Balance, // ������� - ��� - ������
    &pppADC_Stretch, // ������� - ��� - ��������
    &pppADC_Shift,   // ������� - ��� - ��� ����
    Page::Name::Debug_ADC, &pageDebug, FuncActive, EmptyPressPage, FuncDrawPage, FuncRegSetPage
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_Channels_BandwidthA(bool)
{
    FPGA::SetBandwidth(Chan::A);
}

DEF_CHOICE_7( cChannels_BandwidthA,                                                                                                                     //--- ������� - ������ - ������ 1 ---
    "������ 1", "Bandwidth 1",
    "����� ����� ������� ������, ������� ����� ����������� � �����1-������ ��� ������ �������� ������",
    "Here you can select the bandwidth, which will operate in CHANNEL1-Bandwidth when set to Full",
    "������", "Full",
    "20���", "20MHz",
    "100���", "100MHz",
    "200���", "200MHz",
    "350���", "350MHz",
    "650���", "650MHz",
    "750���", "750MHz",
    BANDWIDTH_DEBUG(Chan::A), ppChannels, FuncActive, OnChanged_Channels_BandwidthA, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChanged_Channels_BandwidthB(bool)
{
    FPGA::SetBandwidth(Chan::B);
}

DEF_CHOICE_7(       cChannels_BandwidthB,                                                                        //--- ������� - ������ - ������ 2 ---
    "������ 2", "Bandwidth 2",
    "����� ����� ������� ������, ������� ����� ����������� � �����2-������ ��� ������ �������� ������",
    "Here you can select the bandwidth, which will operate in CHANNEL2-Bandwidth when set to Full",
    "������", "Full",
    "20���",  "20MHz",
    "100���", "100MHz",
    "200���", "200MHz",
    "350���", "350MHz",
    "650���", "650MHz",
    "750���", "750MHz",
    BANDWIDTH_DEBUG(Chan::B), ppChannels, FuncActive, OnChanged_Channels_BandwidthB, FuncDraw
)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_2( ppChannels, // -V641                                                                                                                                   //--- ������� - ������ ---
    "������", "CHANNELS",
    "",
    "",
    &cChannels_BandwidthA,   // ������� - ������ - ������ 1
    &cChannels_BandwidthB,   // ������� - ������ - ������ 2
    Page::Name::Debug_Channels, &pageDebug, FuncActive, EmptyPressPage, FuncDrawPage, FuncRegSetPage
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       cStats,                                                                                                                                    //--- ������� - ���������� ---
    "����������", "Statistics",
    "����������/�� ���������� �����/����, ������ � �������, ���������� �������� � ���������� ����������� � ������/���������� ����������� � ������ "
    "��������",
    "To show/not to show a time/shot, frames per second, quantity of signals with the last settings in memory/quantity of the signals kept in memory",
    "�� ����������", "Hide",
    "����������",    "Show",
    SHOW_STAT, pageDebug, FuncActive, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void PageDebug::OnChanged_DisplayOrientation(bool)
{
    Display::SetOrientation(DISPLAY_ORIENTATION);
}

DEF_CHOICE_2(       cDisplayOrientation,                                                                                                                       //--- ������� - ���������� ---
    "����������", "DisplayOrientation",
    "������������� ���������� �������",
    "Sets display orientation",
    "������",   "Direct",
    "��������", "Back",
    DISPLAY_ORIENTATION, pageDebug, FuncActive, PageDebug::OnChanged_DisplayOrientation, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static int16 pred;

static void OnChanged_Pred()
{
    FPGA::pred = (uint16)(~pred);
}

DEF_GOVERNOR(       mgPred,                                                                                             //--- ������� - ���������� ---
    "����������", "",
    "", "",
    pred, 0, 15000, pageDebug, FuncActive, OnChanged_Pred, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static int16 post;

static void OnChanged_Post()
{
    FPGA::post = (uint16)~post;
}

DEF_GOVERNOR(       mgPost,                                                                                            //--- ������� - ����������� ---
    "�����������", "",
    "", "",
    post, 0, 15000, pageDebug, FuncActive, OnChanged_Post, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Settings_Exit()
{
    Display::SetDrawMode(DrawMode::Auto, 0);
}

DEF_SMALL_BUTTON_EXIT(  bSettings_Exit,                                                                          //--- ������� - ��������� - ����� ---
    ppSettings, FuncActive, OnPress_Settings_Exit, DrawSB_Exit
)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void DebugShowSetInfo_Draw()
{
    Painter::BeginScene(Color::BACK);
    Painter::DrawRectangle(0, 0, 319, 239, Color::FILL);

    int x0 = 30;
    int y0 = 25;
    int dY = 10;
    int y = y0 - dY;

#define Y_AND_INCREASE (y += dY, y)
#define DRAW_TEXT(str)                  Painter::DrawText(x0, Y_AND_INCREASE, str)
#define DRAW_FORMAT(str, value)         String(str, value).Draw(x0, Y_AND_INCREASE)
#define DRAW_FORMAT2(str, val1, val2)   String(str, val1, val2).Draw(x0, Y_AND_INCREASE)

    //Painter_DrawFormatText(x0, Y_AND_INCREASE, "������ �������� ��������� %d", sizeof(set));
    DRAW_FORMAT("������ �������� ��������� : %d", sizeof(set));
    String("�������������� ���������:").Draw(x0, Y_AND_INCREASE);

    int x = String("rShiftAdd :").Draw(x0, Y_AND_INCREASE) + 5;

    int ddY = 0;

    for (int type = 0; type < 2; type++)
    {
        for (int ch = 0; ch < 2; ch++)
        {
            for (int range = 0; range < Range::Number; ++range)
            {
                String("%d", NRST_RSHIFT_ADD(ch, range, type)).Draw(x + range * 20, y + dY * ddY);
            }
            ddY++;
        }
    }

    y += dY * 3;

    DRAW_FORMAT("correctionTime : %d", NRST_CORRECTION_TIME);
    DRAW_FORMAT2("balanceADC : %d %d", NRST_BALANCE_ADC_A, NRST_BALANCE_ADC_B);
    DRAW_FORMAT("numAveForRand : %d", NRST_NUM_AVE_FOR_RAND);

    pString s[3] = {"���������", "��������� �������������", "������ �������"};
    DRAW_FORMAT("balanceADCtype : %s", (NRST_BALANCE_ADC_TYPE < 3 ? s[NRST_BALANCE_ADC_TYPE] : "!!! ������������ �������� !!!"));
    DRAW_FORMAT("stretchADCtype : %s", (NRST_STRETCH_ADC_TYPE < 3 ? s[NRST_STRETCH_ADC_TYPE] : "!!! ������������ �������� !!!"));

    x = String("stretchADC :").Draw(x0, Y_AND_INCREASE) + 5;

    for (int ch = 0; ch < 2; ch++)
    {
        for (int num = 0; num < 3; num++)
        {
            String("%d", NRST_STRETCH_ADC(ch, num)).Draw(x + num * 20, y + dY * ch);
        }
    }

    y += dY;

#define DRAW_STRETCH(name) DRAW_FORMAT2(#name " : %d %d", set.nrst_##name[0], set.nrst_##name[1])

    /*
    DRAW_STRETCH(AddStretch20mV);
    DRAW_STRETCH(AddStretch50mV);
    DRAW_STRETCH(AddStretch100mV);
    DRAW_STRETCH(AddStretch2V);
    */

    DRAW_FORMAT("numSmoothForRand : %d", NRST_NUM_SMOOTH_FOR_RAND);

    Menu::Draw();
    Painter::EndScene();
}

static void OnPress_Settings(bool)
{
    Display::SetDrawMode(DrawMode::Auto, DebugShowSetInfo_Draw);
}

DEF_PAGE_SB( ppSettings, // -V641 // -V1027                                                                                                                     //--- ������� - ��������� ---
    "���������", "SETTINGS",
    "�������� ���������� � ����������",
    "Show settings information",
    &bSettings_Exit,            // ������� - ��������� - �����
    0,
    0,
    0,
    0,
    0,
    Page::Name::SB_Debug_Settings, &pageDebug, FuncActive, OnPress_Settings, FuncDrawPage, FuncRegSetPage
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsActive_SaveFirmware()
{
    return FDrive::IsConnected();
}

static void OnPress_SaveFirmware()
{
    Display::FuncOnWaitStart(DICT(DSaveFirmware), false);

    StructForWrite structForWrite;

    FDrive::OpenNewFileForWrite("S8-54.bin", &structForWrite);

    uint8 *address = (uint8 *)0x08020000;   // -V566
    uint8 *endAddress = address + 128 * 1024 * 3;

    int sizeBlock = 512;

    while (address < endAddress)
    {
        FDrive::WriteToFile(address, sizeBlock, &structForWrite);
        address += sizeBlock;
    }

    FDrive::CloseFile(&structForWrite);

    Display::FuncOnWaitStop();

    Display::ShowWarning(Warning::FirmwareSaved);
}

DEF_BUTTON(         bSaveFirmware,                                                                                  //--- ������� - ����. �������� ---
    "����. ��������", "Save firmware",
    "���������� �������� - �������� 5, 6, 7 ����� ������� 3 � 128 ��, ��� �������� ���������",
    "Saving firmware - sectors 5, 6, 7 with a total size of 3 x 128 kB, where the program is stored",
    pageDebug, IsActive_SaveFirmware, OnPress_SaveFirmware, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_SerialNumber_Exit()
{
    OnPressSB_Exit();
    FREE_EXTRAMEM();
}

DEF_SMALL_BUTTON_EXIT(  bSerialNumber_Exit,                                                                            //--- ������� - �/� - ����� ---
    ppSerialNumber, FuncActive, OnPress_SerialNumber_Exit, DrawSB_Exit
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_SerialNumber_Change()
{
    ACCESS_EXTRAMEM(StructForSN, s);
    ++s->curDigt;
    s->curDigt %= 2;
    Painter::ResetFlash();
}

static void Draw_SerialNumber_Change(int x, int y)
{
    Painter::SetFont(Font::Type::_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, SYMBOL_TAB);
    Painter::SetFont(Font::Type::_8);
}

DEF_SMALL_BUTTON(   bSerialNumber_Change,                                                                           //--- ������� - �/� - �������� ---
    "��������", "Insert",
    "��������� �������� ������",
    "Inserts the chosen symbol",
    ppSerialNumber, FuncActive, OnPress_SerialNumber_Change, Draw_SerialNumber_Change
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_SerialNumber_Save()
{
    ACCESS_EXTRAMEM(StructForSN, s);

    char stringSN[20];

    snprintf(stringSN, 19, "%02d %04d", s->number, s->year);

    if (!OTPmem::SaveSerialNumber(stringSN))
    {
        Display::ShowWarning(Warning::FullyCompletedOTP);
    }
}

static void Draw_SerialNumber_Save(int x, int y)
{
    Painter::SetFont(Font::Type::_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_SAVE_TO_MEM);
    Painter::SetFont(Font::Type::_8);
}

DEF_SMALL_BUTTON(   bSerialNumber_Save,                                                                            //--- ������� - �/� - ��������� ---
    "���������", "Save",
    "���������� �������� ����� � OTP",
    "Records the serial number in OTP",
    ppSerialNumber, FuncActive, OnPress_SerialNumber_Save, Draw_SerialNumber_Save
)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Draw_EnterSerialNumber()
{
    int x0 = Grid::Left() + 40;
    int y0 = Grid::Top() + 20;
    int width = Grid::Width() - 80;
    int height = 160;

    Painter::DrawRectangle(x0, y0, width, height, Color::FILL);
    Painter::FillRegion(x0 + 1, y0 + 1, width - 2, height - 2, Color::BACK);

    int deltaX = 10;

    ACCESS_EXTRAMEM(StructForSN, s);

    bool selNumber = s->curDigt == 0;

    char buffer[20];
    snprintf(buffer, 19, "%02d", s->number);

    Color colorText = Color::FILL;
    Color colorBackground = Color::BACK;

    if (selNumber)
    {
        colorText = Color::FLASH_01;
        colorBackground = Color::FLASH_10;
    }

    int y = y0 + 50;

    Painter::SetColor(colorText);
    int x = Painter::DrawTextOnBackground(x0 + deltaX, y, buffer, colorBackground);

    colorText = Color::FLASH_01;
    colorBackground = Color::FLASH_10;

    if (selNumber)
    {
        colorText = Color::FILL;
        colorBackground = Color::BACK;
    }

    snprintf(buffer, 19, "%04d", s->year);

    Painter::SetColor(colorText);
    Painter::DrawTextOnBackground(x + 5, y, buffer, colorBackground);

    // ������ ������� ���������� �� ���������� ����� � OTP-������ ��� ������

    int allShots = 0;
    String serialNumber = OTPmem::GetSerialNumber(&allShots);

    String("������� ���������� ����� %s", serialNumber.CString()[0] == 0 ? "-- ----" : serialNumber.CString()).Draw(x0 + deltaX, y0 + 130, Color::FILL);

    String("�������� ����� ��� %d �������", allShots).Draw(x0 + deltaX, y0 + 100, Color::FILL);
}

static void OnPress_SerialNumber(bool)
{
    Display::SetAddDrawFunction(Draw_EnterSerialNumber);
    MALLOC_EXTRAMEM(StructForSN, s);
    s->number =  1; // -V522
    s->year = 2017;
    s->curDigt = 0;
}

static bool HandlerKey_SerialNumber(KeyEvent event)
{
    pFuncVpIII p = event.IsAboveZero() ? (Math::CircleIncrease<int>) : (Math::CircleDecrease<int>);

    ACCESS_EXTRAMEM(StructForSN, s);

    if (s->curDigt == 0)
    {
        p(&s->number, 1, 99);
    }
    else
    {
        p(&s->year, 2016, 2050);
    }
    Sound::GovernorChangedValue();

    return true;
}

DEF_PAGE_SB( ppSerialNumber, // -V641 // -V1027                                                                                                                       //--- ������� - �/� ---
    "�/�", "S/N",
    "������ ��������� ������ � OTP-������. ��������!!! ��P-������ - ������ � ����������� �������.",
    "Serial number recording in OTP-memory. ATTENTION!!! OTP memory is a one-time programming memory.",
    &bSerialNumber_Exit,            // ������� - �/� - �����
    &bSerialNumber_Change,          // ������� - �/� - �������
    0,
    0,
    0,
    &bSerialNumber_Save,            // ������� - �/� - ���������
    Page::Name::SB_Debug_SerialNumber, &pageDebug, FuncActive, OnPress_SerialNumber, FuncDrawPage, HandlerKey_SerialNumber
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_EraseData()
{
    Display::FuncOnWaitStart(DICT(DDeleteFromMemory), false);
    EEPROM::DeleteAllData();
    Display::FuncOnWaitStop();
}

DEF_BUTTON(         bEraseData,                                                                                     //--- ������� - ������� ������ ---
    "������� �����", "Erase data",
    "������� ���������� ������ �� ����",
    "Erase all saved datas from EEPROM",
    pageDebug, FuncActive, OnPress_EraseData, FuncDraw
)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_5( pageDebug, // -V641                                                                                                                                             //--- ������� ---
    "�������", "DEBUG",
    "",
    "",
    PageDebug::PageConsole::pointer,    ///< ������� - �������
    &ppADC,                             ///< ������� - ���
    PageDebug::PageRand::pointer,       ///< ������� - ����-���
    &ppChannels,		                ///< ������� - ������
    &cStats,			                ///< ������� - ����������
//    &cDisplayOrientation,       ///< ������� - ����������
//    &mgPred,			          ///< ������� - ����������
//    &mgPost,			          ///< ������� - �����������
//    &ppSettings,		          ///< ������� - ���������
//    &bSaveFirmware,             ///< ������� - ����. ��������
//    &ppSerialNumber,            ///< ������� - �/�
//    &bEraseData,                ///< ������� - ������� ������
    Page::Name::Debug, Menu::pageMain, FuncActive, EmptyPressPage, FuncDrawPage, FuncRegSetPage
)
