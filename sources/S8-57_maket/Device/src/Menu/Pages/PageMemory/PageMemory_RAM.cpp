#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "Data/DataStorage.h"
#include "Display/Painter.h"
#include "Display/Symbols.h"
#include "FlashDrive/FlashDrive.h"
#include "FPGA/FPGA.h"
#include "Hardware/Sound.h"
#include "Menu/MenuItems.h"
#include "Menu/Pages/Include/PageMemory.h"
#include "Settings/Settings.h"
#include "Settings/SettingsMemory.h"
#include "Utils/Math.h"
#include "Utils/Values.h"
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const PageBase pageRAM;
const PageBase *PageMemory::PageRAM::pointer = &pageRAM;



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void OnPress_Last_Next()
{
    Math::CircleIncrease<int16>((int16 *)&NUM_RAM_SIGNAL, 0, (int16)(DataStorage::NumElementsInStorage() - 1));
}

static void Draw_Last_Next(int x, int y)
{
    Painter::SetFont(Font::Type::_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, '\x64');
    Painter::SetFont(Font::Type::_8);
}

DEF_SMALL_BUTTON(bLast_Next,                                                                               //--- ������ - ��������� - ��������� ---
    "���������", "Next",
    "������� � ���������� �������",
    "Go to the next signal",
    pageRAM, FuncActive, OnPress_Last_Next, Draw_Last_Next
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Last_Prev()
{
    Math::CircleDecrease<int16>((int16 *)&NUM_RAM_SIGNAL, 0, (int16)(DataStorage::NumElementsInStorage() - 1));
}

static void Draw_Last_Prev(int x, int y)
{
    Painter::SetFont(Font::Type::_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, SYMBOL_BACKSPACE);
    Painter::SetFont(Font::Type::_8);
}

DEF_SMALL_BUTTON(bLast_Prev,                                                                              //--- ������ - ��������� - ���������� ---
    "����������", "Previous",
    "������� � ����������� �������",
    "Go to the previous signal",
    pageRAM, FuncActive, OnPress_Last_Prev, Draw_Last_Prev
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Last_SaveToROM()
{
    ((Page *)PageMemory::PageRAM::pointer)->SetAsCurrent();
    MODE_WORK = ModeWork::ROM;      // ��������� � ������ ����������� ��
    EXIT_FROM_ROM_TO_RAM = 1;       // ���� ������� �� ������������� ��� ����, �����:
                                    // 1 - �� ������� ������ ����� �� �������� "����� ��" �������� � "���������", � �� � �������� ����;
                                    // 2 - ��� ����, ����� �� �������� "����� ��" �������� �� ��������� ��������� ������, � ��������� �� �������� 
                                    // "���������";
                                    // 3 - ����� � Data_Load() ������������� ��������������� ���������.
}

static void Draw_Last_SaveToROM(int x, int y)
{
    Painter::SetFont(Font::Type::_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_ROM);
    Painter::SetFont(Font::Type::_8);
}

DEF_SMALL_BUTTON(bLast_SaveToROM,                                                                           //--- ������ - ��������� - ����� �� ---
    "����� ��", "Internal storage",
    "������� ��� ������, ����� ��������� ������ �� ���������� ������������ ����������",
    "Press this button to keep a signal in an internal memory",
    pageRAM, FuncActive, OnPress_Last_SaveToROM, Draw_Last_SaveToROM
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Last_SaveToDrive()
{
    EXIT_FROM_SETNAME_TO = RETURN_TO_LAST_MEM;
    PageMemory::SaveSignalToFlashDrive();
}

static void Draw_Last_SaveToDrive(int x, int y)
{
    if (FDrive::IsConnected())
    {
        Painter::SetFont(Font::Type::_UGO2);
        Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_FLASH_DRIVE_BIG);
        Painter::SetFont(Font::Type::_8);
    }
}

DEF_SMALL_BUTTON(bLast_SaveToDrive,                                                                        //--- ������ - ��������� - ��������� ---
    "���������", "Save",
    "������ ���������� �������� ��� ������������� ������� ��. ��������� ��������� ������ �� ������ ��",
    "Click this button to save the signal on the external FLASH",
    pageRAM, FuncActive, OnPress_Last_SaveToDrive, Draw_Last_SaveToDrive
)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void OnPress_RAM(bool enter)
{
    if (enter)
    {
        NUM_RAM_SIGNAL = 0;
        RUN_FPGA_BEFORE_SB = FPGA::IsRunning() ? 1U : 0U;
        FPGA::Stop(false);
        MODE_WORK = ModeWork::RAM;
    }
    else
    {
        MODE_WORK = ModeWork::Dir;
        FPGA::OnPressStart();
    }
}

static void OnDraw_RAM()
{
    int width = 40;
    int height = 10;
    Painter::FillRegion(Grid::Right() - width, Grid::Top(), width, height, Color::BACK);
    Painter::DrawRectangle(Grid::Right() - width, Grid::Top(), width, height, Color::FILL);
    Integer(NUM_RAM_SIGNAL + 1).ToString(false, 3).Draw(Grid::Right() - width + 2, Grid::Top() + 1);
    String("/").Draw(Grid::Right() - width + 17, Grid::Top() + 1);
    Integer((int)DataStorage::NumElementsInStorage()).ToString(false, 3).Draw(Grid::Right() - width + 23, Grid::Top() + 1);
}

static bool HandlerKey_RAM(KeyEvent event)
{
    Key::E key = event.key;

    if (DataStorage::NumElementsInStorage() > 1)
    {
        Sound::RegulatorSwitchRotate();
    }
    if (key == Key::Up || key == Key::Right)
    {
        OnPress_Last_Next();
    }
    else
    {
        OnPress_Last_Prev();
    }

    return true;
}

DEF_PAGE_4( pageRAM, // -V641 // -V1027                                                                                                                          //--- ������ - ��������� ---
    "���������", "LATEST",
    "������� � ����� ������ � ���������� ����������� ���������",
    "Transition to an operating mode with the last received signals",
    &bLast_Next,                        ///< ������ - ��������� - ���������
    &bLast_Prev,                        ///< ������ - ��������� - ����������
    &bLast_SaveToROM,                   ///< ������ - ��������� - ����� ��
    &bLast_SaveToDrive,                 ///< ������ - ��������� - ���������
    Page::Name::SB_Memory_Last, PageMemory::pointer, FuncActive, OnPress_RAM, OnDraw_RAM, HandlerKey_RAM
)
