#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "Menu/Pages/Include/PageMemory.h"
#include "Tables.h"
#include "Data/Reader.h"
#include "Data/DataStorage.h"
#include "Display/Grid.h"
#include "Display/Symbols.h"
#include "Display/Painter.h"
#include "FlashDrive/FlashDrive.h"
#include "FPGA/FPGA.h"
#include "Hardware/EEPROM.h"
#include "Hardware/Sound.h"
#include "FlashDrive/FileManager.h"
#include "Menu/Menu.h"
#include "Menu/Pages/Include/Definition.h"
#include "Utils/Dictionary.h"
#include "Utils/Math.h"
#include "Utils/StringUtils.h"
#include "Utils/Values.h"
#include "Utils/CommonFunctions.h"
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const PageBase pageMemory;
extern const PageBase ppDrive;
extern const PageBase pppDrive_Manager;
extern const PageBase pppDrive_Mask;
extern const PageBase pSetName;

const PageBase *PageMemory::pointer = &pageMemory;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawSetMask();  // ��� ������� ������, ����� ������ ����� ������� �����.
static void DrawFileMask(int x, int y);
static void DrawSetName();  // ��� ������� ������, ����� ����� ������ ��� ����� ��� ����������

//static void SaveSignalToIntMemory();
       void OnMemExtSetMaskNameRegSet(int angle, int maxIndex);


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static bool IsActive_Points()
{
    return SET_PEAKDET_DIS;
}

void PageMemory::OnChanged_Points(bool active)
{
    // ���� ������� ������� ��������, �� �� ����� ����������� ������
    if (SET_PEAKDET_EN && !active)
    {
        Display::ShowWarning(Warning::WrongModePeackDet);
        return;
    }

    int width = Grid::Width();

    FPGA::Reset();

    if (SET_PEAKDET_EN)
    {
        width *= 2;
    }

    if (TPOS_IS_LEFT)
    {
        SHIFT_IN_MEMORY = 0;
    }
    else if (TPOS_IS_CENTER)
    {
        SHIFT_IN_MEMORY = (int16)(NUM_BYTES_SET / 2 - width / 2);
    }
    else // TPOS_IS_RIGHT
    {
        SHIFT_IN_MEMORY = (int16)(NUM_BYTES_SET - width - 2);
    }

    FPGA::Reset();
    FPGA::SetTShift(SET_TSHIFT);
    FPGA::Reset();
}

static pString namesLengthMemory[] = {"512", "1k", "2k", "4k", "8k", "16k", "32k"};

DEF_CHOICE_5( cPoints,                                                                                                                                              //--- ������ - ����� ---
    "����� ������", "Mem length",
    "����� ���������� �������� ��� ����������� ��������. "
    "��� ���������� ���������� �������� ����������� ���������� ���������� � ������ ��������.",
    "Choice of number of counting for the saved signals. "
    "At increase in number of counting the quantity of the signals kept in memory decreases.",
    namesLengthMemory[0], namesLengthMemory[0],
    namesLengthMemory[1], namesLengthMemory[1],
    namesLengthMemory[2], namesLengthMemory[2],
    namesLengthMemory[3], namesLengthMemory[3],
    namesLengthMemory[4], namesLengthMemory[4],
    FPGA_ENUM_POINTS, pageMemory, IsActive_Points, PageMemory::OnChanged_Points, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void Draw_Drive_Manager_Tab(int x, int y)
{
    Painter::SetFont(Font::Type::_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_TAB);
    Painter::SetFont(Font::Type::_8);
}

DEF_SMALL_BUTTON(   bDrive_Manager_Tab,                                                                                                           //--- ������ - ����� �� - ������� - Tab ---
    "Tab", "Tab",
    "������� ����� ���������� � �������",
    "The transition between the directories and files",
    pppDrive_Manager, FuncActive, FileManager::PressSB_Tab, Draw_Drive_Manager_Tab
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void Draw_Drive_Manager_LevelUp(int x, int y)
{
    Painter::SetFont(Font::Type::_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, '\x48');
    Painter::SetFont(Font::Type::_8);
}

DEF_SMALL_BUTTON(   bDrive_Manager_LevelUp,                                                  //--- ������ - ����� �� - ������� - ����� �� �������� ---
    "����� �� ��������", "Leave from directory",
    "������� � ������������ �������",
    "Transition to the parental catalog",
    pppDrive_Manager, FuncActive, FileManager::PressSB_LevelUp, Draw_Drive_Manager_LevelUp
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void Draw_Drive_Manager_LevelDown(int x, int y)
{
    Painter::SetFont(Font::Type::_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, '\x4a');
    Painter::SetFont(Font::Type::_8);
}

DEF_SMALL_BUTTON(   bDrive_Manager_LevelDown,                                                  //--- ������ - ����� �� - ������� - ����� � ������� ---
    "����� � �������", "Enter in directory",
    "������� � ��������� �������",
    "Transition to the chosen catalog",
    pppDrive_Manager, FuncActive, FileManager::PressSB_LevelDown, Draw_Drive_Manager_LevelDown
)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool IsActive_Drive_Manager()
{
    return FDrive::IsConnected();
}

void PageMemory::OnPress_Drive_Manager(bool)
{
    if (FDrive::IsConnected())
    {
        FDrive::Mount();
        Display::SetDrawMode(DrawMode::Auto, FileManager::Draw);
        FM_NEED_REDRAW = FM_REDRAW_FULL;
    }
    else
    {
        Display::ShowWarning(Warning::WarnNeedForFlashDrive);
    }
}

/*
DEF_PAGE_SB(        pppDrive_Manager,                                                                               // ������ - ����� �� - ������� ///
    "�������", "DIRECTORY",
    "��������� ������ � �������� ������� ������������� ����������",
    "Provides access to the file system of the connected drive",
    &bDrive_Manager_Exit,       // ������ - ����� �� - ������� - �����
    &bDrive_Manager_Tab,        // ������ - ����� �� - ������� - Tab
    0,
    0,
    &bDrive_Manager_LevelUp,    // ������ - ����� �� - ������� - ����� �� ��������
    &bDrive_Manager_LevelDown,  // ������ - ����� �� - ������� - ����� � �������
    Page::Name::SB_Memory_Drive_Manager, &ppDrive, IsActive_Drive_Manager, PageMemory::OnPress_Drive_Manager, FuncDrawPage, FileManager::RotateRegSet
)
*/

DEF_PAGE_3( pppDrive_Manager, // -V641                                                                                                                  //--- ������ - ����� �� - ������� ---
    "�������", "DIRECTORY",
    "��������� ������ � �������� ������� ������������� ����������",
    "Provides access to the file system of the connected drive",
    &bDrive_Manager_Tab,        // ������ - ����� �� - ������� - Tab
    &bDrive_Manager_LevelUp,    // ������ - ����� �� - ������� - ����� �� ��������
    &bDrive_Manager_LevelDown,  // ������ - ����� �� - ������� - ����� � �������
    Page::Name::SB_Memory_Drive_Manager, &ppDrive, IsActive_Drive_Manager, PageMemory::OnPress_Drive_Manager, FuncDrawPage, FileManager::HandlerKey
)


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2(       cDrive_Name,                                                                                                                      //--- ������ - ����� �� - ��� ����� ---
    "��� �����", "File name"
    ,
    "����� ����� ������������ ������ ��� ���������� �� ������� ����������:\n"
    "\"�� �����\" - ����� ��������� ������������� �� ������� �������� ����� (����. ����� ����),\n"
    "\"�������\" - ������ ��� ��� ����� ����� �������� �������"
    ,
    "Sets the mode name when saving files to an external drive:\n"
    "\"By mask\" - files are named automatically advance to the entered mask(seq.Menu),\n"
    "\"Manual\" - each time the file name must be specified manually"
    ,
    "�� �����", "Mask",
    "�������",  "Manually",
    FILE_NAMING_MODE, ppDrive, FuncActive, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Drive_Mask_Delete()
{
    FILE_NAME_MASK[0] = '\0';
}

static void Draw_Delete(int x, int y)
{
    Painter::SetFont(Font::Type::_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_DELETE);
    Painter::SetFont(Font::Type::_8);
}

DEF_SMALL_BUTTON( bDrive_Mask_Delete,                                                                                                           //--- ������ - ����� �� - ����� - ������� ---
    "�������", "Delete",
    "������� ��� �������� �������",
    "Deletes all entered symbols",
    pppDrive_Mask, FuncActive, OnPress_Drive_Mask_Delete, Draw_Delete
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Drive_Mask_Backspace()
{
    int size = (int)std::strlen(FILE_NAME_MASK);
    if (size > 0)
    {
        if (size > 1 && FILE_NAME_MASK[size - 2] == 0x07)
        {
            FILE_NAME_MASK[size - 2] = '\0';
        }
        else
        {
            FILE_NAME_MASK[size - 1] = '\0';
        }
    }
}

static void Draw_Backspace(int x, int y)
{
    Painter::SetFont(Font::Type::_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_BACKSPACE);
    Painter::SetFont(Font::Type::_8);
}

DEF_SMALL_BUTTON(   bDrive_Mask_Backspace,                                                             //--- ������ - ����� �� - ����� - Backspace ---
    "Backspace", "Backspace",
    "������� ��������� �������� ������",
    "Deletes the last entered symbol",
    pppDrive_Mask, FuncActive, OnPress_Drive_Mask_Backspace, Draw_Backspace
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Drive_Mask_Insert()
{
    int index = INDEX_SYMBOL;
    int size = (int)std::strlen(FILE_NAME_MASK);
    if (size == MAX_SYMBOLS_IN_FILE_NAME - 1)
    {
        return;
    }
    if (index < 0x41)
    {
        FILE_NAME_MASK[size] = Tables::Get(index)[0];
        FILE_NAME_MASK[size + 1] = '\0';
    }
    else
    {
        index -= 0x40;
        if (index == 0x07)  // ��� %nN - ��������� ������
        {
            if (size < MAX_SYMBOLS_IN_FILE_NAME - 2 && size > 0)
            {
                if (FILE_NAME_MASK[size - 1] >= 0x30 && FILE_NAME_MASK[size - 1] <= 0x39) // ���� ����� ������� �����
                {
                    FILE_NAME_MASK[size] = FILE_NAME_MASK[size - 1] - 0x30;
                    FILE_NAME_MASK[size - 1] = 0x07;
                    FILE_NAME_MASK[size + 1] = '\0';
                }
            }
        }
        else
        {
            FILE_NAME_MASK[size] = (char)index;
            FILE_NAME_MASK[size + 1] = '\0';
        }
    }
}

static void Draw_Insert(int x, int y)
{
    Painter::SetFont(Font::Type::_UGO2);
    Painter::Draw4SymbolsInRect(x + 2, y + 2, SYMBOL_INSERT);
    Painter::SetFont(Font::Type::_8);
}

DEF_SMALL_BUTTON( bDrive_Mask_Insert,                                                                                                          //--- ������ - ����� �� - ����� - �������� ---
    "��������", "Insert",
    "��������� ��������� ������",
    "Inserts the chosen symbol",
    pppDrive_Mask, FuncActive, OnPress_Drive_Mask_Insert, Draw_Insert
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2( cDrive_SaveAs,                                                                                                                      //--- ������ - ����� �� - ��������� ��� ---
    "��������� ���", "Save as"
    ,
    "���� ������ ������� \"�����������\", ������ ����� ������� � ������� �������� � ����������� ����� � ����������� BMP\n"
    "���� ������ ������� \"�����\", ������ ����� ������� � ������� �������� � ��������� ���� � ����� � ���������� TXT"
    ,
    "If you select \"Image\", the signal will be stored in the current directory in graphic file with the extension BMP\n"
    "If you select \"Text\", the signal will be stored in the current directory as a text file with the extension TXT"
    ,
    "�����������", "Image",
    "�����",       "Text",
    MODE_SAVE, ppDrive, FuncActive, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2( cDrive_ModeBtnMemory,                                                                                                               //--- ������ - ����� �� - ��� �� ������ ---
    "��� �� ������", "Mode btn MEMORY",
    "",
    "",
    "����",       "Menu",
    "����������", "Save",
    MODE_BTN_MEMORY, ppDrive, FuncActive, FuncChangedChoice, FuncDraw
)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool IsActive_Drive_Mask()
{
    return FILE_NAMING_MODE_MASK;
}

static void OnPress_Drive_Mask(bool)
{
    Display::SetAddDrawFunction(DrawSetMask);
}

static void DrawSetMask()
{
    int x0 = Grid::Left() + 40;
    int y0 = Grid::Top() + 20;
    int width = Grid::Width() - 80;
    int height = 160;

    Painter::DrawRectangle(x0, y0, width, height, Color::FILL);
    Painter::FillRegion(x0 + 1, y0 + 1, width - 2, height - 2, Color::BACK);

    int index = 0;
    int position = 0;
    int deltaX = 10;
    int deltaY0 = 5;
    int deltaY = 12;

    // ������ ������� ����� ����������� ��������
    while (Tables::Get(index)[0] != ' ')
    {
        Tables::DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0);
        index++;
        position++;
    }

    // ������ ������ ����� � ������
    position = 0;
    while (Tables::Get(index)[0] != 'a')
    {
        Tables::DrawStr(index, x0 + deltaX + 50 + position * 7, y0 + deltaY0 + deltaY);
        index++;
        position++;
    }

    // ������ ������ ����� ����� ��������
    position = 0;
    while (Tables::Get(index)[0] != '%')
    {
        Tables::DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0 + deltaY * 2);
        index++;
        position++;
    }

    // ������ ������ �����������
    position = 0;
    while (index < (Tables::Size() / 4))
    {
        Tables::DrawStr(index, x0 + deltaX + 26 + position * 20, y0 + deltaY0 + deltaY * 3);
        index++;
        position++;
    }

    DrawFileMask(x0 + deltaX, y0 + 65);

    static pString strings[] =
    {
        "%y - ���, %m - �����, %d - ����",
        "%H - ����, %M - ������, %S - �������",
        "%nN - ���������� �����, ���",
        "n - ����������� ���������� ������ ��� N"
    };

    deltaY--;
    Painter::SetColor(Color::FILL);
    for (int i = 0; i < (int)sizeof(strings) / 4; i++)
    {
        String(strings[i]).Draw(x0 + deltaX, y0 + 100 + deltaY * i);
    }
}

static void DrawFileMask(int x, int y)
{
    char *ch = FILE_NAME_MASK;

    Painter::SetColor(Color::FILL);
    while (*ch != '\0')
    {
        if (*ch >= 32)
        {
            x = Painter::DrawChar(x, y, *ch);
        }
        else
        {
            if (*ch == 0x07)
            {
                x = Painter::DrawChar(x, y, '%');
                x = Painter::DrawChar(x, y, (char)(0x30 | *(ch + 1)));
                x = Painter::DrawChar(x, y, 'N');
                ch++;
            }
            else
            {
                x = String(Tables::Get(*ch + 0x40)).Draw(x, y);
            }
        }
        ch++;
    }
    Painter::FillRegion(x, y, 5, 8, Color::FLASH_10);
}

static bool HandlerKey_Drive_Mask(KeyEvent event)
{
    Key::E key = event.key;

    int angle = (key == Key::Up || key == Key::Right) ? 1 : -1;

    OnMemExtSetMaskNameRegSet(angle, Tables::Size() / 4);

    return true;
}


/*
DEF_PAGE_SB( pppDrive_Mask,                                                                                                                               //--- ������ - ����� �� - ����� ---
    "�����", "MASK",
    "����� ����� ����� ��� ��������������� ���������� ������",
    "Input mode mask for automatic file naming",
    &bDrive_Mask_Exit,      // ������ - ����� �� - ����� - �����
    &bDrive_Mask_Delete,    // ������ - ����� �� - ����� - �������
    0,
    0,
    &bDrive_Mask_Backspace, // ������ - ����� �� - ����� - Backspace
    &bDrive_Mask_Insert,    // ������ - ����� �� - ����� - ��������
    Page::Name::SB_Memory_Drive_Mask, &ppDrive, IsActive_Drive_Mask, OnPress_Drive_Mask, FuncDrawPage, OnRegSet_Drive_Mask
)
*/


DEF_PAGE_3( pppDrive_Mask, // -V641                                                                                                                       //--- ������ - ����� �� - ����� ---
    "�����", "MASK",
    "����� ����� ����� ��� ��������������� ���������� ������",
    "Input mode mask for automatic file naming",
    &bDrive_Mask_Delete,    // ������ - ����� �� - ����� - �������
    &bDrive_Mask_Backspace, // ������ - ����� �� - ����� - Backspace
    &bDrive_Mask_Insert,    // ������ - ����� �� - ����� - ��������
    Page::Name::SB_Memory_Drive_Mask, &ppDrive, IsActive_Drive_Mask, OnPress_Drive_Mask, FuncDrawPage, HandlerKey_Drive_Mask
)


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2( cDrive_Autoconnect,                                                                                                               //--- ������ - ����� �� - ��������������� ---
    "���������.", "AutoConnect",
    "E��� \"���\", ��� ����������� �������� ���������� ���������� �������������� ������� �� �������� ������ - ����� ��",
    "If \"Enable\", when you connect an external drive is automatically transferred to the page MEMORY - Ext.StorageOld",
    DISABLE_RU, DISABLE_EN,
    ENABLE_RU,  ENABLE_EN,
    FLASH_AUTOCONNECT, ppDrive, FuncActive, FuncChangedChoice, FuncDraw
)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_6( ppDrive,  //-V641                                                                                                                                     //--- ������ - ����� �� ---
    "����� ��", "EXT STORAGE",
    "������ � ������� ������������ �����������.",
    "Work with external storage device.",
    &pppDrive_Manager,       ///< ������ - ����� �� - �������
    &cDrive_Name,            ///< ������ - ����� �� - ��� �����
    &pppDrive_Mask,          ///< ������ - ����� �� - �����
    &cDrive_SaveAs,          ///< ������ - ����� �� - ��������� ���
    &cDrive_ModeBtnMemory,   ///< ������ - ����� �� - ��� �� ������
    &cDrive_Autoconnect,     ///< ������ - ����� �� - ���������������
    Page::Name::Memory_Drive, &pageMemory, FuncActive, EmptyPressPage, FuncDrawPage, FuncRegSetPage
)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_PAGE_4( pageMemory, // -V641                                                                                                                                             //--- ������ ---
    "������", "MEMORY",
    "������ � ������� � ���������� �������.",
    "Working with external and internal memory.",
    &cPoints,                           ///< ������ - �����
    PageMemory::PageRAM::pointer,      ///< ������ - ���������
    PageMemory::PageROM::pointer,      ///< ������ - ����� ��
    &ppDrive,                           ///< ������ - ����� ��
    Page::Name::Memory, Menu::pageMain, FuncActive, EmptyPressPage, FuncDrawPage, FuncRegSetPage
)

void PageMemory::SaveSignalToFlashDrive()
{
    if (FDrive::IsConnected())
    {
        if (FILE_NAMING_MODE_MANUAL)
        {
            Display::SetAddDrawFunction(DrawSetName);
        }
        else
        {
            NEED_SAVE_TO_FLASHDRIVE = 1;
        }
    }
    else
    {
        EXIT_FROM_SETNAME_TO = 0;
    }
}

static void DrawSetName()
{
    int x0 = Grid::Left() + 40;
    int y0 = Grid::Top() + 60;
    int width = Grid::Width() - 80;
    int height = 80;

    Painter::DrawRectangle(x0, y0, width, height, Color::FILL);
    Painter::FillRegion(x0 + 1, y0 + 1, width - 2, height - 2, Color::BACK);

    int index = 0;
    int position = 0;
    int deltaX = 10;
    int deltaY0 = 5;
    int deltaY = 12;

    // ������ ������� ����� ����������� ��������
    while (Tables::Get(index)[0] != ' ')
    {
        Tables::DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0);
        index++;
        position++;
    }

    // ������ ������ ����� � ������
    position = 0;
    while (Tables::Get(index)[0] != 'a')
    {
        Tables::DrawStr(index, x0 + deltaX + 50 + position * 7, y0 + deltaY0 + deltaY);
        index++;
        position++;
    }

    // ������ ������ ����� ����� ��������
    position = 0;
    while (Tables::Get(index)[0] != '%')
    {
        Tables::DrawStr(index, x0 + deltaX + position * 7, y0 + deltaY0 + deltaY * 2);
        index++;
        position++;
    }

    int x = String(FILE_NAME).Draw(x0 + deltaX, y0 + 65, Color::FILL);

    Painter::FillRegion(x, y0 + 65, 5, 8, Color::FLASH_10);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_SetName_Exit()
{
    OnPressSB_Exit();

    if (EXIT_FROM_SETNAME_TO == RETURN_TO_DISABLE_MENU)
    {
        //Menu::PagePointerFromName(Page::Name::SB_Memory_SetName)->ShortPressOnItem(0);
    }
    else if (EXIT_FROM_SETNAME_TO == RETURN_TO_LAST_MEM)
    {
    }
    else if (EXIT_FROM_SETNAME_TO == RETURN_TO_INT_MEM)
    {
    }
    else
    {
        //
    }

    EXIT_FROM_SETNAME_TO = RETURN_TO_DISABLE_MENU;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_SetName_Delete()
{
    FILE_NAME[0] = '\0';
}

DEF_SMALL_BUTTON( bSetName_Delete,                                                                                                                              //--- ��� ����� - ������� ---
    "�������", "Delete",
    "������� ��� �������� �������",
    "Deletes all entered characters",
    pSetName, FuncActive, OnPress_SetName_Delete, Draw_Delete
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_SetName_Backspace()
{
    int size = (int)std::strlen(FILE_NAME);
    if (size > 0)
    {
        FILE_NAME[size - 1] = '\0';
    }
}

DEF_SMALL_BUTTON( bSetName_Backspace,                                                                                                                         //--- ��� ����� - Backspace ---
    "Backspace", "Backspace",
    "������� ��������� ������",
    "Delete the last character",
    pSetName, FuncActive, OnPress_SetName_Backspace, Draw_Backspace
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_SetName_Insert()
{
    int size = (int)std::strlen(FILE_NAME);
    if (size < MAX_SYMBOLS_IN_FILE_NAME - 1)
    {
        FILE_NAME[size] = Tables::Get(INDEX_SYMBOL)[0];
        FILE_NAME[size + 1] = '\0';
    }
}

DEF_SMALL_BUTTON( bSetName_Insert,                                                                                                                             //--- ��� ����� - �������� ---
    "��������", "Insert",
    "������ ��������� ������",
    "Print the next character",
    pSetName, FuncActive, OnPress_SetName_Insert, Draw_Insert
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_SetName_Save()
{
    if (FDrive::IsConnected())
    {
        OnPress_SetName_Exit();
        NEED_SAVE_TO_FLASHDRIVE = 1;
    }
}

static void Draw_SetName_Save(int x, int y)
{
    if (FDrive::IsConnected())
    {
        Painter::SetFont(Font::Type::_UGO2);
        Painter::Draw4SymbolsInRect(x + 2, y + 1, SYMBOL_FLASH_DRIVE_BIG);
        Painter::SetFont(Font::Type::_8);
    }
}

DEF_SMALL_BUTTON(bSetName_Save,                                                                                     //--- ��� ����� - ��������� ---
    "���������", "Save",
    "���������� �� ���� ��� �������� ������",
    "Saving to flashdrive with the specified name",
    pSetName, FuncActive, OnPress_SetName_Save, Draw_SetName_Save
)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool HandlerKey_SetName(KeyEvent event)
{
    OnMemExtSetMaskNameRegSet(event.Delta(), Tables::Size() / 4 - 7);

    return true;
}

void OnMemExtSetMaskNameRegSet(int angle, int maxIndex)
{
    static pFuncVpI8I8I8 func[3] =
    {
        Math::CircleDecrease<int8>,
        Math::CircleDecrease<int8>,
        Math::CircleIncrease<int8>
    };

    Painter::ResetFlash();
    if (INDEX_SYMBOL > maxIndex)
    {
        INDEX_SYMBOL = (int8)(maxIndex - 1);
    }
    func[Math::Sign(angle) + 1](&INDEX_SYMBOL, 0, (int8)(maxIndex - 1));
    Sound::RegulatorSwitchRotate();

}

DEF_PAGE_4(pSetName, // -V641                                                                                                                // �������� ���������� ��� ����� ����� ����� ///
    "", "",
    "",
    "",
    &bSetName_Delete,       // ���� ����� ����� - �������
    &bSetName_Backspace,    // ���� ����� ����� - Backspace
    &bSetName_Insert,       // ���� ����� ����� - ��������
    &bSetName_Save,         // ���� ����� ����� - ���������
    Page::Name::SB_Memory_SetName, 0, FuncActive, EmptyPressPage, FuncDrawPage, HandlerKey_SetName
)
