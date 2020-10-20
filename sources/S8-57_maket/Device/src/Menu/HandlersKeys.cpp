#include "defines.h"
#include "device.h"
#include <stm32f4xx.h>
#include "log.h"
#include "HandlersKeys.h"
#include "Display/Painter.h"
#include "FPGA/FPGA.h"
#include "Hardware/Timer.h"
#include "Menu/Menu.h"
#include "Menu/Pages/Include/PageChannels.h"
#include "Menu/Pages/Include/PageFunction.h"
#include "Menu/Pages/Include/PageMeasures.h"
#include "Menu/Pages/Include/PageMemory.h"
#include "Menu/Pages/Include/PageService.h"
#include "Menu/Pages/Include/PageTime.h"
#include "Menu/Pages/Include/PageTrig.h"
#include "Menu/Pages/Include/PageDisplay.h"
#include "Osci/BottomPart.h"
#include "Settings/Settings.h"
#include "Utils/String.h"
#include <stdio.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef void(*pFuncVCh)(Chan::E);
typedef void(*pFuncVChI)(Chan::E, int);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// �������������� �������
static KeyEvent event;
/// �����, ��������� �������� ����� �������� ��������
static Chan drawingChan = Chan(Chan::A);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void RangeMoreB();
/// ���������� ���������� �������������� ������
static void FuncRelease();
/// ���������� ����������� ������� �������������� ������
static void FuncLong();
/// ����� ���������� ��������� ��������� ������ - �������� ��� ��������
static void OnChangeParameterChannel(pFuncVChI, Chan::E, int);
/// ������� ��������� ���������� ������
static void DrawParametersChannel();
/// ����� ���������� ��������� ��������� ����������
static void OnChangeParameterTime(pFuncVI, int);
/// ������� ��������� ��������� ����������
static void DrawParametersTime();

static void OpenPage(const PageBase *page);


/// ������ ����������
static void Empty();
/// ���������� ������� �������
static void Arrow();
static void RShiftLessA();
static void RShiftMoreA();
static void RShiftLessB();
static void RShiftMoreB();
static void RangeLessA();
static void RangeMoreA();
static void RangeLessB();
static void TShiftLess();
static void TShiftMore();
static void TBaseLess();
static void TBaseMore();
static void TrigLevLess();
static void TrigLevMore();
static void EnterRelease();
static void EnterLong();
static void ChannelA();
static void ChannelB();
static void Function();
static void Measure();
static void Memory();
static void Service();
static void Time();
static void Start();
static void Trig();
static void Display();


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Handlers::Process(KeyEvent e)
{
    event = e;

    static const pFuncVV func[Key::Number][4] =
    {                   // Press        Repead       Release        Long
        /* None        */ {Empty,       Empty,       Empty,         Empty},
        /* Function    */ {Empty,       Empty,       Function,      Empty},
        /* Measure     */ {Measure,     Measure,     Measure,       Measure},
        /* Memory      */ {Memory,      Memory,      Memory,        Memory},
        /* Service     */ {Service,     Service,     Service,       Service},
        /* ChannelA    */ {ChannelA,    Empty,       Empty,         Empty},
        /* ChannelB    */ {ChannelB,    Empty,       Empty,         Empty},
        /* Time        */ {Time,        Time,        Time,          Time},
        /* Start       */ {Start,       Empty,       Empty,         Empty},
        /* Trig        */ {Trig,        Trig,        Trig,          Trig},
        /* Display     */ {Display,     Display,     Display,       Display},
        /* RangeMoreA  */ {RangeMoreA,  Empty,       Empty,         Empty},
        /* RangeLessA  */ {RangeLessA,  Empty,       Empty,         Empty},
        /* RShiftMoreA */ {RShiftMoreA, RShiftMoreA, Empty,         Empty},
        /* RShiftLessA */ {RShiftLessA, RShiftLessA, Empty,         Empty},
        /* RangeMoreB  */ {RangeMoreB,  Empty,       Empty,         Empty},
        /* RangeLessB  */ {RangeLessB,  Empty,       Empty,         Empty},
        /* RShiftMoreB */ {RShiftMoreB, RShiftMoreB, Empty,         Empty},
        /* RShiftLessB */ {RShiftLessB, RShiftLessB, Empty,         Empty},
        /* TBaseMore   */ {TBaseMore,   Empty,       Empty,         Empty},
        /* TBaseLess   */ {TBaseLess,   Empty,       Empty,         Empty},
        /* TShiftMore  */ {TShiftMore,  TShiftMore,  Empty,         Empty},
        /* TShiftLess  */ {TShiftLess,  TShiftLess,  Empty,         Empty},
        /* TrigLevMore */ {TrigLevMore, TrigLevMore, Empty,         Empty},
        /* TrigLevLess */ {TrigLevLess, TrigLevLess, Empty,         Empty},
        /* Left        */ {Arrow,       Arrow,       Arrow,         Arrow},
        /* Right       */ {Arrow,       Arrow,       Arrow,         Arrow},
        /* Up          */ {Arrow,       Arrow,       Arrow,         Arrow},
        /* Down        */ {Arrow,       Arrow,       Arrow,         Arrow},
        /* Enter       */ {Empty,       Empty,       EnterRelease,  EnterLong},
        /* F1          */ {Empty,       Empty,       FuncRelease,   FuncLong},
        /* F2          */ {Empty,       Empty,       FuncRelease,   FuncLong},
        /* F3          */ {Empty,       Empty,       FuncRelease,   FuncLong},
        /* F4          */ {Empty,       Empty,       FuncRelease,   FuncLong},
        /* F5          */ {Empty,       Empty,       FuncRelease,   FuncLong}
    };

    Key::E code = event.key;
    TypePress::E type = event.type;

    if (code < Key::Number && type < TypePress::None)
    {
        func[code][type]();
    }
    else
    {
        /// \todo ��������� �������. ��������, ����� ������ �� ����
        //LOG_WRITE("������ ����� ������� ������");
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void Empty()
{
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void ChangeRShift(Chan::E ch, int delta)
{
    static bool stop[Chan::Number] = { false, false };      // ������� ����, ��� �������� �������� �� ����� - ��� ����� ���� � ������ ���� �������
    static uint timeStop[Chan::Number] = { 0, 0 };          // ����� �������� �������� stop

    if (stop[ch])
    {
        if (TIME_MS - timeStop[ch] > 500)
        {
            stop[ch] = false;
        }
        return;
    }
    else
    {
        FPGA::RShiftChange(ch, delta);
    }

    if (SET_RSHIFT(ch) == RShift::ZERO)
    {
        stop[ch] = true;
        timeStop[ch] = TIME_MS;
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void RShiftLessA()
{
    OnChangeParameterChannel(ChangeRShift, Chan::A, -1);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void RShiftMoreA()
{
    OnChangeParameterChannel(ChangeRShift, Chan::A, 1);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void RShiftLessB()
{
    OnChangeParameterChannel(ChangeRShift, Chan::B, -1);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void RShiftMoreB()
{
    OnChangeParameterChannel(ChangeRShift, Chan::B, 1);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void RangeLessA()
{
    OnChangeParameterChannel(FPGA::ChangeRange, Chan::A, -1);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void RangeMoreA()
{
    OnChangeParameterChannel(FPGA::ChangeRange, Chan::A, +1);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void RangeLessB()
{
    OnChangeParameterChannel(FPGA::ChangeRange, Chan::B, -1);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void RangeMoreB()
{
    OnChangeParameterChannel(FPGA::ChangeRange, Chan::B, +1);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawParametersTime()
{
    int x = Grid::Left() + 5;
    int y = Grid::Top() + 5;
    int width = 126;
    int height = 15;

    Painter::DrawBoundedRegion(x, y, width, height, Color::BACK, Color::FILL);

    String("���� : %s", TBase(SET_TBASE).Name()).Draw(x + 3, y + 3, Color::FILL);

    TShift shift(SET_TSHIFT);

    shift.ToString(SET_TBASE).Draw(x + 64, y + 3);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawParametersChannel()
{
    int x = Grid::Left() + 5;
    int y = Grid::Top() + 5;
    int width = 126;
    int height = 15;
    Chan::E ch = drawingChan.value;

    Painter::DrawBoundedRegion(x, y, width, height, Color::BACK, Color::Channel(ch));

    char buffer[50];
    sprintf(buffer, "%s : %s %s",
        drawingChan.Name(),
        ModeCouple(SET_COUPLE(ch)).UGO(),
        Range(SET_RANGE(ch)).Name()
    );

    Painter::DrawBigText(x + 3, y + 3, 1, buffer, Color::Channel(ch));

    Painter::DrawBigText(x + 80, y + 3, 1, RShift::ToString(SET_RSHIFT(ch), SET_RANGE(ch), SET_DIVIDER(ch)).CString());
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChangeParameterChannel(pFuncVChI func, Chan::E ch, int delta)
{
    if (Device::CurrentMode() == Device::Mode::Osci)
    {
        drawingChan = Chan(ch);

        Display::SetAddDrawFunction(DrawParametersChannel, 5000);
    }

    func(ch, delta);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnChangeParameterTime(pFuncVI func, int delta)
{
    if (Device::CurrentMode() == Device::Mode::Osci)
    {
        Display::SetAddDrawFunction(DrawParametersTime, 5000);
    }

    func(delta);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void TShiftLess()
{
    OnChangeParameterTime(FPGA::TShiftChange, -1);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void TShiftMore()
{
    OnChangeParameterTime(FPGA::TShiftChange, 1);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void TBaseLess()
{
    OnChangeParameterTime(FPGA::TBaseChange, -1);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void TBaseMore()
{
    OnChangeParameterTime(FPGA::TBaseChange, 1);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void FuncRelease()
{
    Menu::ReleaseFunctionalButton(event.key);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void FuncLong()
{
    Menu::LongFunctionalButton(event.key);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void Arrow()
{ 
    if(Menu::OpenedItem()->IsPage())
    {
        if(!Menu::CurrentItem()->ProcessKey(event))
        {
            Menu::OpenedItem()->ProcessKey(event);
        }
    }
    else
    {
        Menu::OpenedItem()->ProcessKey(event);
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void EnterRelease()
{
    if (!Menu::IsShown())
    {
        Menu::Show(true);
    }
    else
    {
        Menu::CloseOpenedItem();
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void EnterLong()
{
    Menu::Show(!Menu::IsShown());
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void TrigLevMore()
{
    FPGA::TrigLevChange(1);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void TrigLevLess()
{
    FPGA::TrigLevChange(-1);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void ChannelA()
{
    //SET_ENABLED_A = !SET_ENABLED_A;

    OpenPage(PageChannelA::pointer);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void ChannelB()
{
    //SET_ENABLED_B = !SET_ENABLED_B;

    OpenPage(PageChannelB::pointer);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void Function()
{
    OpenPage(PageFunction::pointer);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void Measure()
{
    OpenPage(PageMeasures::pointer);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void Memory()
{
    if (MODE_BTN_MEMORY_IS_SAVE)
    {
        if (event.type == TypePress::Press)
        {
            Painter::SaveScreenToDrive();
        }
    }
    else
    {
        OpenPage(PageMemory::pointer);
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void Service()
{
    OpenPage(PageService::pointer);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void Time()
{
    OpenPage(PageTime::pointer);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void Start()
{
    FPGA::OnPressStart();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void Trig()
{
    OpenPage(PageTrig::pointer);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void Display()
{
    OpenPage(PageDisplay::pointer);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OpenPage(const PageBase *page)
{
    ((Page *)page)->SetAsCurrent();
    ((Page *)page)->Open(true);
    Menu::Show(true);
}
