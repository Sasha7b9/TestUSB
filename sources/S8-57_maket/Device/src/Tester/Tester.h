#pragma once
#include "Display/Colors.h"
#include "Settings/SettingsChannel.h"
#include "Utils/String.h"
#include "Command.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TESTER_CONTROL                  (set.test_control)
#define TESTER_CONTROL_IS_U             (TESTER_CONTROL == Tester::Control::Voltage)
#define TESTER_POLARITY                 (set.test_polarity)
#define TESTER_POLARITY_IS_POSITITVE    (TESTER_POLARITY == Tester::Polarity::Positive)
#define TESTER_STEP_U                   (set.test_stepU)
#define TESTER_STEP_I                   (set.test_stepI)
#define TESTER_NUM_SMOOTH               (set.test_smooth)
#define TESTER_VIEW_MODE                (set.test_viewMode)
#define TESTER_VIEW_MODE_IS_LINES       (TESTER_VIEW_MODE == Tester::ViewMode::Lines)




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tester
{
public:
    static const int NUM_STEPS = 5;
    /// ������������� ���������� �����
    static void Init();

    static void Enable();

    static void Disable();

    static void Update();

    static void ProcessStep();
    /// ��������� ���������� �� Settings
    static void LoadPolarity();
    /// ������������� ��� ��������� ���������� � ����������� � ����������� Settings
    static void LoadStep();

    static int DeltaRShiftA();

    struct Graphics
    {
    public:
        static void Update();

        static void SetPoints(int numStep, const uint8 dx[TESTER_NUM_POINTS], const uint8 dy[TESTER_NUM_POINTS]);
    };

private:
    /// ��������� FPGA � ������������ � �������������� �����������
    static void LoadFPGA();
    /// ������� ������ ��������� ���������
    static void ReadData();
    /// ������� ���
    static int step;
    /// ��� ��������� ����������
    static float stepU;
    /// ������������� � true �������� ��������, ��� ������ ����� �������
    static bool enabled;

    static uint8 data[Chan::Number][NUM_STEPS][TESTER_NUM_POINTS];

public:
    /// ��� ����� ��������� � ������-���������� - ����������� ��� �����
    struct Control
    {
        enum E
        {
            Voltage,
            Current
        } value;
        explicit Control(E v) : value(v) {};
    };

    struct Polarity
    {
        enum E
        {
            Positive,
            Negative
        } value;
        explicit Polarity(E v) : value(v) {};
    };

    struct StepU
    {
        enum E
        {
            _100mV,
            _500mV
        } value;
        explicit StepU(E v) : value(v) {};
    };

    struct StepI
    {
        enum E
        {
            _4mA,
            _20mA
        } value;
        explicit StepI(E v) : value(v) {};
    };

    struct Smoothing
    {
        enum E
        {
            _1,
            _2,
            _3,
            _4,
            _5
        } value;
        explicit Smoothing(E v) : value(v) {};
    };

    struct ViewMode
    {
        enum E
        {
            Lines,
            Points
        } value;
        explicit ViewMode(E v) : value(v) {};
    };

    /// �������
    struct Scale
    {
        enum E
        {
            _2uA,
            _5uA,
            _10uA,
            _20uA,
            _50uA,
            _100uA,
            _200uA,
            _500uA,
            _1mA,
            _2mA,
            _5mA,
            _10mA,
            _20mA,
            Number
        } value;
        Chan::E ch;
        explicit Scale(Range::E range, Chan::E _ch) : value((E)range), ch(_ch) {};
        pString ToString() const;
    };

    /// ��������
    struct Shift
    {
        Shift(uint16 rShift, Chan::E _ch) : shift(rShift), ch(_ch) {};
        String ToString(Scale::E scale);
    private:
        uint16 shift;
        Chan::E ch;
    };
};
