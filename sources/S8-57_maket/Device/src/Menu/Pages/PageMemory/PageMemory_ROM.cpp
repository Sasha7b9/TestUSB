#include "stdafx.h" 
#ifndef WIN32
#include "defines.h"
#include "Data/Reader.h"
#include "Display/Painter.h"
#include "Display/Symbols.h"
#include "FlashDrive/FlashDrive.h"
#include "Hardware/EEPROM.h"
#include "Hardware/Sound.h"
#include "Menu/MenuItems.h"
#include "Menu/Pages/Include/PageMemory.h"
#include "Settings/Settings.h"
#include "Settings/SettingsMemory.h"
#include "Utils/Dictionary.h"
#include "Utils/Math.h"
#include "Utils/Values.h"
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const PageBase pageROM;
const PageBase *PageMemory::PageROM::pointer = &pageROM;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawMemoryWave(int num, bool exist);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void OnPress_Internal_ShowAlways()
{
    ALWAYS_SHOW_ROM_SIGNAL = ALWAYS_SHOW_ROM_SIGNAL ? 0U : 1U;
}

static void Draw_Internal_ShowAlways_Yes(int x, int y)
{
    Painter::SetFont(Font::Type::_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x66');
    Painter::SetFont(Font::Type::_8);
}

static void Draw_Internal_ShowAlways_No(int x, int y)
{
    Painter::SetFont(Font::Type::_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x68');
    Painter::SetFont(Font::Type::_8);
}

static void Draw_Internal_ShowAlways(int x, int y)
{
    if (ALWAYS_SHOW_ROM_SIGNAL == 0)
    {
        Draw_Internal_ShowAlways_No(x, y);
    }
    else
    {
        Draw_Internal_ShowAlways_Yes(x, y);
    }
}

DEF_SMALL_BUTTON_HINTS_2(bInternal_ShowAlways,                                                      //--- ������ - ����� �� - ���������� ������ ---
    "���������� ������", "To show always",
    "��������� ������ ���������� ��������� ���������� ������ ������ ��������",
    "Allows to show always the chosen kept signal over the current",
    pageROM, FuncActive, OnPress_Internal_ShowAlways, Draw_Internal_ShowAlways,
    Draw_Internal_ShowAlways_Yes, { "���������� ��������� ������ �� ���������� ������ ������ ��������",
                                    "to show the chosen signal from internal memory over the current" },
    Draw_Internal_ShowAlways_No, { "������ �� ���������� ������ ����� ������ � ������ ������ � ���������� ������������ �����������",
                                    "the signal from internal memory is visible only in an operating mode with an internal memory" }
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Internal_ModeShow()
{
    Math::CircleIncrease<int8>((int8 *)&SHOW_IN_INT, 0, 2);
}

static void Draw_Internal_ModeShow_Direct(int x, int y)
{
    Painter::SetFont(Font::Type::_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x6a');
    Painter::SetFont(Font::Type::_8);
}


static void Draw_Internal_ModeShow_Saved(int x, int y)
{
    Painter::SetFont(Font::Type::_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x6c');
    Painter::SetFont(Font::Type::_8);
}

static void Draw_Internal_ModeShow_Both(int x, int y)
{
    String("���").Draw(x + 1, y + 5);
    Painter::DrawVLine(x + 1, y + 2, y + 14, Color::BACK);
    Painter::DrawVLine(x + 2, y + 6, y + 11, Color::FILL);
}

static void Draw_Internal_ModeShow(int x, int y)
{
    if (SHOW_IN_INT_DIRECT)
    {
        Draw_Internal_ModeShow_Direct(x, y);
    }
    else if (SHOW_IN_INT_SAVED)
    {
        Draw_Internal_ModeShow_Saved(x, y);
    }
    else
    {
        Draw_Internal_ModeShow_Both(x, y);
    }
}

DEF_SMALL_BUTTON_HINTS_3(bInternal_ModeShow,                                                              //--- ������ - ����� �� - ��� ������� ---
    "��� �������", "Type of a signal",
    "���������� ���������� ��� ������� ������ � ������ ����� ��",
    "Show recorded or current signal in mode Internal Memory",
    pageROM, FuncActive, OnPress_Internal_ModeShow, Draw_Internal_ModeShow,
    Draw_Internal_ModeShow_Direct, { "�� ������� ������� ������", "on the display current signal" },
    Draw_Internal_ModeShow_Saved, { "�� ������� ���������� ������", "on the display the kept signal" },
    Draw_Internal_ModeShow_Both, { "�� ������� ��� �������", "on the display both signals" }
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Internal_Delete()
{
    Display::FuncOnWaitStart(DICT(DDeleteFromMemory), false);
    EEPROM::DeleteData(NUM_ROM_SIGNAL);
    Display::FuncOnWaitStop();
}

static void Draw_Internal_Delete(int x, int y)
{
    Painter::SetFont(Font::Type::_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_DELETE);
    Painter::SetFont(Font::Type::_8);
}

DEF_SMALL_BUTTON(bInternal_Delete,                                                                            //--- ������ - ����� �� - ������� ---
    "�������", "Delete",
    "������� ��������� ������ �� ����������� ������������� ����������",
    "Removes the selected signal from the internal storage device",
    pageROM, FuncActive, OnPress_Internal_Delete, Draw_Internal_Delete
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void SaveSignalToIntMemory()
{
    // ������� � ��������� DS, DATA_A, DATA_B ������ �� ��� ��� ��������� ��������� ������, � ����������� �� ����, �� ������ ������ ����� � 
    // "������-����� ��"
    Reader::ReadFromRAM(EXIT_FROM_ROM_TO_RAM ? NUM_RAM_SIGNAL : 0, 0, false);

    if (DS)                                             // ���� ���� ��� ���������
    {
        EEPROM::SaveData(NUM_ROM_SIGNAL, DS, IN_A, IN_B);   // �� ��������� ������ �� DS, DATA_A, DATA_B �� ����� NUM_ROM_SIGNAL � ����
        Display::ShowWarning(Warning::SignalIsSaved);
    }
}

static void OnPress_Internal_SaveToMemory()
{
    Display::FuncOnWaitStart(DICT(DStoredInMemory), false);
    SaveSignalToIntMemory();
    Display::FuncOnWaitStop();
}

static void Draw_Internal_SaveToMemory(int x, int y)
{
    Painter::SetFont(Font::Type::_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_SAVE_TO_MEM);
    Painter::SetFont(Font::Type::_8);
}

DEF_SMALL_BUTTON(bInternal_SaveToMemory,                                                           //--- ������ - ����� �� - ��������� � ������ ---
    "��������� � ������", "Save to memory",
    "��������� ������ �� ���������� ������������ ����������",
    "To keep a signal in an internal memory",
    pageROM, FuncActive, OnPress_Internal_SaveToMemory, Draw_Internal_SaveToMemory
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Internal_SaveToDrive()
{
    EXIT_FROM_SETNAME_TO = RETURN_TO_INT_MEM;
    PageMemory::SaveSignalToFlashDrive();
}

static void Draw_Internal_SaveToDrive(int x, int y)
{
    if (FDrive::IsConnected())
    {
        Painter::SetFont(Font::Type::_UGO2);
        Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_FLASH_DRIVE_BIG);
        Painter::SetFont(Font::Type::_8);
    }
}

DEF_SMALL_BUTTON(bInternal_SaveToDrive,                                                                                                     //--- ������ - ����� �� - ��������� �� ������ ---
    "��������� �� ������", "Save to disk",
    "��������� ������ �� ������",
    "Save signal to flash drive",
    pageROM, FuncActive, OnPress_Internal_SaveToDrive, Draw_Internal_SaveToDrive
)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void OnPress_Internal(bool)
{
    MODE_WORK = ModeWork::ROM;
}

static void OnDraw_Internal()
{
    // ������ �������� ��������� ������

    bool exist[MAX_NUM_SAVED_WAVES] = { false };

    EEPROM::GetDataInfo(exist);

    for (int i = 0; i < MAX_NUM_SAVED_WAVES; i++)
    {
        DrawMemoryWave(i, exist[i]);
    }
}

static void DrawMemoryWave(int num, bool exist)
{
    int x = Grid::Left() + 2 + num * 12;
    int y = Grid::FullBottom() - 10;
    int width = 12;
    Painter::FillRegion(x, y, width, 10, num == NUM_ROM_SIGNAL ? Color::FLASH_10 : Color::BACK);
    Painter::DrawRectangle(x, y, width, 10, Color::FILL);
    Painter::SetColor(num == NUM_ROM_SIGNAL ? Color::FLASH_01 : Color::FILL);
    if (exist)
    {
        Integer(num + 1).ToString(false, 2).Draw(x + 2, y + 1);
    }
    else
    {
        String("\x88").Draw(x + 3, y + 1);
    }
}

static bool HandlerKey_Internal(KeyEvent event)
{
    Key::E key = event.key;

    Sound::RegulatorSwitchRotate();

    if (key == Key::Left || key == Key::Down)
    {
        Math::CircleDecrease<int8>((int8 *)&NUM_ROM_SIGNAL, 0, MAX_NUM_SAVED_WAVES - 1);
    }
    else if (key == Key::Right || key == Key::Up)
    {
        Math::CircleIncrease<int8>((int8 *)&NUM_ROM_SIGNAL, 0, MAX_NUM_SAVED_WAVES - 1);
    }
    else
    {
        // ��������� ������ �� ������������
    }

    Painter::ResetFlash();

    return true;
}


DEF_PAGE_5( pageROM, // -V641                                                                                                                                     //--- ������ - ����� �� ---
    "����� ��", "INT STORAGE",
    "������� � ����� ������ � ���������� �������",
    "Transition to an operating mode with internal memory",
    &bInternal_ShowAlways,      ///< ������ - ����� �� - ���������� ������
    &bInternal_ModeShow,        ///< ������ - ����� �� - ��� �������
    &bInternal_Delete,          ///< ������ - ����� �� - �������
    &bInternal_SaveToMemory,    ///< ������ - ����� �� - ���������
    &bInternal_SaveToDrive,     ///< ������ - ����� �� - ��������� �� ������
    Page::Name::SB_Memory_Internal, PageMemory::pointer, FuncActive, OnPress_Internal, OnDraw_Internal, HandlerKey_Internal
)
