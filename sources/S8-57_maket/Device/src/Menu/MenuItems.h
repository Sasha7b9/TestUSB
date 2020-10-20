#pragma once
#include "Display/Display.h"
#include "Display/Colors.h"
#include "MenuItemsDefs.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @addtogroup Menu
 *  @{
 *  @defgroup MenuItems Menu Items
 *  @{
 */

extern int8 gCurDigit;

#define MENU_ITEMS_ON_DISPLAY       5   ///< ������� ������� ���� ���������� �� ������ �� ���������.


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ����� ����� ��� ���� ����� ��������� ����
#define COMMON_PART_MENU_ITEM                                                                           \
    uint8           type;           /* ��� ����� */                                                     \
    int8            num;            /* ����� ��������� ��� Choice ��� ����� ��������� ��� Page*/        \
    bool            isPageSB;       /* ���� true, �� ��� �������� ����� ������ */                       \
    uint8           name;           /* ��� �� ������������ Page::Name */                                  \
    const PageBase  *keeper;        /* ����� ��������, ������� �����������. ��� Page_Main = 0 */        \
    pFuncBV         funcOfActive;   /* ������� �� ������ ������� */                                     \
    const char      *titleHint[4]   /* �������� �������� �� ������� � ���������� ������. ����� ��������� ��� ������ ������ */

class PageBase;

#define IS_PAGE(item)           (item->type == Control::Type::Page)
#define NOT_PAGE(item)          (item->type != Control::Type::Page)
#define IS_PAGE_SB(item)        (item->isPageSB)
#define IS_CHOICE(item)         (item->type == Control::Type::Choice)
#define IS_CHOICE_REG(item)     (item->type == Control::Type::ChoiceReg)
#define NOT_CHOICE_REG(item)    (item->type != Control::Type::ChoiceReg)
#define IS_GOVERNOR(item)       (item->type == Control::Type::Governor)
#define NOT_GOVERNOR(item)      (item->type != Control::Type::Governor)
#define IS_GOVERNOR_COLOR(item) (item->type == Control::Type::GovernorColor)
#define IS_TIME(item)           (item->type == Control::Type::Time)
    
#define KEEPER(item)            ((PageBase *)item->keeper)


class Control
{
public:
    COMMON_PART_MENU_ITEM;
    /// ���������� ������ � �������� ��������� �������� Choice ��� Page::Name
    int HeightOpened() const;
    /// @brief ���������� true, ���� ������� ���� item ������ (��������� �� �� ����� ������� ����. ��� �������, ��� ��������, ��� ������� 
    /// �������������� ������� ���� ����� Choice ��� Governor
    bool IsShade() const;
    /// ���������� true, ���� ������, ��������������� �������� ���� item, ��������� � ������� ���������
    bool IsPressed() const;
    /// �������/��������� �������
    void SetCurrent(bool active);
    /// ���������� true, ���� ������� ���� �� ������ item ������
    bool IsOpened() const;

    void Open(bool open);
    /// ���������� �������� ��������, ��� ��� �������� �� ������� �������
    String Title() const;
    /// ��������� ������� ������
    bool ProcessKey(KeyEvent event);
    /// ���������� ��� "��������" �������
    void ShortPress();
    /// ���������� ��� "�������" �������
    void LongPress();
    /// ���������� true, ���� ������� ��������� � �������� ��������� (��������� �� ������ ����������)
    bool IsAcitve() { return funcOfActive(); };

    void Draw(int x, int y, bool opened);

    bool IsCurrentItem() const;
    /// ������� ���������� � ���
    void LogInfo() const;
    /// ���������� ����������� ����������, ��������������� ��� �������� ���������
    char GetSymbol();
    /// ���������� ������ ��������
    int Width() { return 320 / 5; };
    /// ���������� true, ���� ��� �������� - Page
    bool IsPage() const;

    /// ������ ���� ������� ����
    struct Type
    {
        enum E
        {
            None,
            Choice,         ///< ����� ������ - ��������� ������� ���� �� ���������� �������� ��������.
            Button,         ///< ������.
            Page,           ///< ��������.
            Governor,       ///< ��������� - ��������� ������� ����� ����� �������� �������� �� ������� ��������� ����������. �������� �� ����� ��������� [ -(1 << 16) / 2 , (1 << 16) / 2]
            Time,           ///< ��������� ������ �����.
            GovernorColor,  ///< ��������� ������� ����.
            ChoiceReg,      ///< ������� ������, � ������� ����� �������������� �� �������, � ������
            DrawButton,     ///< ������ ��� ������ ����� ������
            ChoiceParameter,
            Number
        } value;

        explicit Type(E v) : value(v) {};
        operator uint8() const { return (uint8)value; };
    };
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Page ///

typedef bool(*pFuncBKE)(KeyEvent);

/// ��������� �������� ����.
class PageBase
{
public:
    COMMON_PART_MENU_ITEM;
    const Control * const *items;   ///< ����� ��������� �� ������ ���� �������� (� ������� ��������)
                                    ///< ��� �������� ����� ������  ����� �������� 6 ���������� �� SButton : 0 - K_Enter, 1...5 - K_1...K_5
    pFuncVB     funcOnEnterExit;    ///< ����� ���������� ��� ������� �� �������� �������� � ��� ������ �� ���� �������� �� ����������
    pFuncVV     funcOnDraw;         ///< ����� ���������� ����� ��������� ������
    pFuncBKE    funcRegSet;         ///< ���������� ��� ������� ������ �����/������. ���� ����������, ���������� true
    bool CurrentItemIsOpened() const;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SMALL_BUTTON_FROM_PAGE(page, numButton)     ((SButton *)((Page *)page)->items[numButton])

class Page : public Control
{
public:
    const Control * const *items;   ///< ����� ��������� �� ������ ���� �������� (� ������� ��������)
                                    ///< ��� �������� ����� ������  ����� �������� 6 ���������� �� SButton : 0 - K_Enter, 1...5 - K_1...K_5
    pFuncVB     funcOnEnterExit;    ///< ����� ���������� ��� ������� �� �������� �������� � ��� ������ �� ���� �������� �� ����������
    pFuncVV     funcOnDraw;         ///< ����� ���������� ����� ��������� ������
    pFuncBKE    funcKey;            ///< � �������� ����� ������ ���������� ��� ������� �������
    /// ���������� true, ���� ������� ������� �������� ������
    bool CurrentItemIsOpened() const;
    /// D��������� ����� ���������� � �������� �� ������ page
    int NumSubPages() const;        
    /// ���������� ���������� ��������� � �������� �� ������ page
    int NumItems() const;
    /// ���������� ������� ������ ��������.
    void SetAsCurrent();
    /// ���������� ����� ������� ����������� (���, ������� ������������ � ������ ������)
    int8 CurrentSubPage() const;
    /// ���������� ������� ������ �����������
    void SetCurrentSubPage(int8 pos);
    ///< ������������� ������� ��������� ������ ����
    void SetPosActItem(int8 pos);
    ///< ���������� ��������� �� �������� ������� ��������
    Control *Item(int numElement) const;
    /// \todo ���������� ������� ������� �������� �������� �� ������ page �� ������. ���� ������� ����������� 0, ��� ����� 0, ���� ������� 
    /// ����������� 1, ��� ����� 5 � �.�.
    int PosItemOnLeft();
    /// �������� ������� ��������� ������� ������ ��� ������ numItem �������� page
    void ShortPressOnItem(uint numItem);
    /// ���������� ������� �������� �������� ������� page
    int8 PosCurrentItem() const;
    /// �������� ����� ������� ����������� �� �������� delta
    void ChangeSubPage(int delta);
    /// ���������� � �������� �����������
    void Draw(int x, int y, bool opened);
    /// ��������� ������� ������
    bool ProcessKey(KeyEvent event);

    void DrawTitle(int x, int y);

    void DrawItems(int x, int y);

    static int ItemOpenedPosX(Control *item);

    void DrawPagesUGO(int right, int bottom);

    void DrawNestingPage(int left, int bottom);

    struct Name
    {
        enum E
        {
            Main,                       ///< ������� �������� ����

            Function,                   ///< �������
            Function_FrequencyCounter,  ///< ������� - ����������
            Function_FFT,               ///< ������� - ������
            Function_Multimeter,        ///< ������� - ����������
            Function_Tester,            ///< ������� - ������
            Function_Recorder,          ///< ������� - �����������
            Function_Recorder_Source,   ///< ������� - ����������� - ��������
            Function_Recorder_Record,   ///< ������� - ����������� - ������
            Function_Recorder_Play,     ///< ������� - ����������� - ��������

            Measures,                   ///< ���������
            Measures_Auto,              ///< ��������� - �������
            Measures_Auto_Tune,         ///< ��������� - ������� - ���������
            Measures_Cursors,           ///< ��������� - �������
            Measures_Cursors_Set,       ///< ��������� - ������� - ����������

            Display,                    ///< ������
            Display_Accumulation,       ///< ������� - ����������
            Display_Average,            ///< ������� - ����������
            Display_Grid,               ///< ������� - �����
            Display_Settings,           ///< ������� - ���������
            Display_Settings_Colors,    ///< ������� - ��������� - �����

            ChannelA,                   ///< ����� 1
            ChannelB,                   ///< ����� 2
            Trig,                       ///< �����
            Trig_Search,                ///< ����� - �����
            Trig_HoldOff,               ///< ����� - ���������
            Time,                       ///< ���������
            Memory,                     ///< ������
            SB_Memory_Last,             ///< ������ - ���������
            SB_Memory_Internal,         ///< ������ - ����� ��
            Memory_Drive,               ///< ������ - ����� ��
            SB_Memory_Drive_Manager,    ///< ������ - ����� �� - �������
            SB_Memory_Drive_Mask,       ///< ������ - ����� �� - �����
            SB_Memory_SetName,          ///< \brief ���������� ����� ����������� ����� �� ������ ��� ������� ����� ����� ��� ��������������� ����� 
                                        ///< ����� �� - ��� �����
            Service,                    ///< ������
            Service_Calibrator,         ///< ������ - ����������
            Service_Ethernet,           ///< ������ - ETHERNET
            Service_Sound,              ///< ������ - ����
            Service_RTC,                ///< ������ - �����
            SB_Service_FFT_Cursors,     ///< ������ - ������ - �������
            SB_Service_Function,        ///< ������ - �������
            SB_Service_Information,     ///< ������ - ����������
            SB_Help,                    ///< ������
            Debug,                      ///< �������
            Debug_Console,              ///< ������� - �������
            Debug_Console_Registers,    ///< ������� - ������� - ��������
            Debug_ADC,                  ///< ������� - ���
            Debug_ADC_Balance,          ///< ������� - ��� - ������
            Debug_ADC_Stretch,          ///< ������� - ��� - ��������
            Debug_ADC_Shift,            ///< ������� - ��� - ��� ����
            Debug_ADC_AltShift,         ///< ������� - ��� - ��� ���� ���
            Debug_Rand,                 ///< ������� - ����-���
            Debug_Channels,             ///< ������� - ������
            SB_Debug_Settings,          ///< ������� - ���������
            SB_Debug_SerialNumber,      ///< ������� - �/�
            Number,
            NoPage
        } value;
        //Name(uint8 v) : value((E)v) {};
    };

    Page::Name::E GetName() const;     ///< ���������� ��� �������� page
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Button ///
/// ��������� ������.
class ButtonBase
{
public:
    COMMON_PART_MENU_ITEM;
    pFuncVV     funcOnPress;        ///< �������, ������� ���������� ��� ������� �� ������.
    pFuncVII    funcForDraw;        ///< ������� ����� ���������� �� ����� ��������� ������.
};

class Button : public Control
{
public:
    pFuncVV     funcOnPress;        ///< �������, ������� ���������� ��� ������� �� ������.
    pFuncVII    funcForDraw;        ///< ������� ����� ���������� �� ����� ��������� ������.
    void Draw(int x, int y);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// SButton ///
struct StructHelpDrawButton
{
    pFuncVII    funcDrawUGO;    ///< ��������� �� ������� ��������� ����������� �������� ������
    pString     helpUGO[2];     ///< ������� � ������� �����������.
};


/// ��������� ������ ��� ��������������� ������ ����.
class SButtonBase
{
public:
    COMMON_PART_MENU_ITEM;
    pFuncVV                         funcOnPress;    ///< ��� ������� ��������� ��� ��������� ������� ������.
    pFuncVII                        funcForDraw;    ///< ��� ������� ���������� ��� ��������� ������ � ����� � ������������ x, y.
    const StructHelpDrawButton     *hintUGO;
    int                             numHints;
};


class SButton : public Control
{
public:
    pFuncVV                     funcOnPress;    ///< ��� ������� ��������� ��� ��������� ������� ������.
    pFuncVII                    funcForDraw;    ///< ��� ������� ���������� ��� ��������� ������ � ����� � ������������ x, y.
    const StructHelpDrawButton *hintUGO; 
    int                         numHints;
    void Draw(int x, int y);
    void DrawHints(int x, int y, int width);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Governor ///
/// ��������� ���������.
class GovernorBase
{
public:
    COMMON_PART_MENU_ITEM;
    int    *cell;
    int     minValue;       ///< ���������� ��������, ������� ����� ��������� ���������.
    int     maxValue;       ///< ������������ ��������.
    pFuncVV funcOfChanged;  ///< �������, ������� ����� �������� ����� ����, ��� �������� ���������� ����������.
    pFuncVV funcBeforeDraw; ///< �������, ������� ���������� ����� ����������
};

class Governor : public Control
{
public:
    int    *cell;
    int     minValue;       ///< ���������� ��������, ������� ����� ��������� ���������.
    int     maxValue;       ///< ������������ ��������.
    pFuncVV funcOfChanged;  ///< �������, ������� ����� �������� ����� ����, ��� �������� ���������� ����������.
    pFuncVV funcBeforeDraw; ///< �������, ������� ���������� ����� ����������
    /// ��������� ������� ������
    bool ProcessKey(KeyEvent event);
    /// ��������� �������� �������� ���������� ��� ���������� �������� ���� ���� Governor (� ����������� �� ����� delta).
    void StartChange(int detla);
    /// ���������� ��������� ������� ��������, ������� ����� ������� governor.
    int NextValue();
    /// ���������� ��������� ������� ��������, ������� ����� ������� governor.
    int PrevValue();
    /// ������������ ��������� ���� ��������.
    float Step();
    /// �������� �������� � ������� ������� ��� ��������� ��������.
    void ChangeValue(int delta);
    /// ��� �������� �������� ������������ ������ �� ��������� �������.
    void NextPosition();
    /// ��� �������� �������� ������������ ������ �� ���������� �������
    void PrevPosition();

    void Draw(int x, int y, bool opened);

    void DrawOpened(int x, int y);

    void DrawClosed(int x, int y);

    void DrawValue(int x, int y);

    char GetSymbol();

private:

    void DrawLowPart(int x, int y, bool shade);
    /// ���������� ����� ��������� � ���� ��� ����� �������� governor. ������� �� ������������� ��������, ������� ����� ��������� governor.
    uint  NumDigits();
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Choice ///
class ChoiceBase
{
public:
    COMMON_PART_MENU_ITEM;
    int8                *cell;
    const char  * const *names;             ///< �������� ������ �� ������� � ���������� ������.
    pFuncVB			    funcOnChanged;      ///< ������� ������ ���������� ����� ��������� �������� ��������.
    pFuncVII            funcForDraw;        ///< ������� ���������� ����� ��������� ��������. 
};

class Choice : public Control
{
public:
    int8       *cell;
    pString    *names;          ///< �������� ������ �� ������� � ���������� ������.
    pFuncVB     funcOnChanged;  ///< ������� ������ ���������� ����� ��������� �������� ��������.
    pFuncVII    funcForDraw;    ///< ������� ���������� ����� ��������� ��������. 
    /// ��������� ������� ��������� �������� �� delta
    void  StartChange(int delta);
    /// ������������ ��������� ���� ��������.
    float Step();
    /// �������� �������� choice � ����������� �� �������� � ����� delta.
    void  ChangeIndex(int delta);
    /// ���������� ���������� ��������� ������ � �������� �� ������ choice
    int   NumSubItems();

    bool ProcessKey(KeyEvent event);

    void  Draw(int x, int y, bool opened);

    void  DrawOpened(int x, int y);

    void  DrawClosed(int x, int y);
    /// ���������� ��� �������� �������� ������ �������� choice, ��� ��� �������� � �������� ���� ���������
    String NameCurrentSubItem() const;
    /// ���������� ��� ���������� �������� ������ �������� choice, ��� ��� �������� � �������� ���� ���������
    const char *NameNextSubItem();

    const char *NamePrevSubItem();
    /// ���������� ��� �������� ������ �������� choice � ������� i ��� ��� �������� � �������� ���� ���������
    String NameSubItem(int i);
    /// ���������� ����, ������� ����� ��������� ������� ������
    static Color ColorMenuField(const Choice *choice);

    char GetSymbol();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// GovernorColor ///
class ColorType;

class GovernorColorBase
{
public:
    COMMON_PART_MENU_ITEM;
    ColorType  *ct;                 ///< ��������� ��� �������� �����.
    pFuncVV     funcOnChanged;      ///< ��� ������� ����� �������� ����� ��������� �������� ��������.
};

class GovernorColor : public Control
{
public:
    ColorType  *ct;                 ///< ��������� ��� �������� �����.
    pFuncVV     funcOnChanged;      ///< ��� ������� ����� �������� ����� ��������� �������� ��������.
    void ChangeValue(int delta);    ///< �������� ������� ����� � governor.
    void Draw(int x, int y, bool opened);
private:
    void DrawOpened(int x, int y);
    void DrawClosed(int x, int y);
    void DrawValue(int x, int y, int delta);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Time ///

#define iEXIT   0
#define iDAY    1
#define iMONTH  2
#define iYEAR   3
#define iHOURS  4
#define iMIN    5
#define iSEC    6
#define iSET    7

class TimeBase
{
public:
    COMMON_PART_MENU_ITEM;
    int8 *curField;   ///< ������� ���� ���������. 0 - �����, 1 - ���, 2 - ���, 3 - ����, 4 - ����, 5 - �����, 6 - ���, 7 - ����������.
    int8 *hours;
    int8 *minutes;
    int8 *seconds;
    int8 *month;
    int8 *day;
    int8 *year;
};

/// ������������� � ���������� �����.
class TimeControl : public Control
{
public:
    int8 *curField;   ///< ������� ���� ���������. 0 - �����, 1 - ���, 2 - ���, 3 - ����, 4 - ����, 5 - �����, 6 - ���, 7 - ����������.
    int8 *hours;
    int8 *minutes;
    int8 *seconds;
    int8 *month;
    int8 *day;
    int8 *year;
    void SetOpened();
    void IncCurrentPosition();
    void SetNewTime();
    void SelectNextPosition();
    void DecCurrentPosition();
    void Draw(int x, int y, bool opened);
    void DrawClosed(int x, int y);
    void DrawOpened(int x, int y);
    char GetSymbol();
};

#define CHOICE_RUN_FUNC_CHANGED(c, val)     \
    if(c->funcOnChanged)                    \
    {                                       \
        c->funcOnChanged(val);              \
    }


typedef void * pVOID;
#define MAX_NUM_ITEMS_IN_PAGE 15
typedef pVOID arrayItems[MAX_NUM_ITEMS_IN_PAGE];

    
#include <Menu/MenuItemsAdd.h>

    
/** @}  @}
 */
