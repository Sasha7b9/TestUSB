#pragma once
#include <stm32f4xx_hal.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MULTI_MEASURE           (set.multi_meas)
#define MULTI_RANGE_AC          (set.multi_rangeAC)
#define MULTI_RANGE_DC          (set.multi_rangeDC)
#define MULTI_RANGE_RESISTANCE  (set.multi_rangeResist)
#define MULTI_AVP               (set.multi_avp)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Multimeter
{
public:

    /// ������������ ��� ���������
    class Graphics
    {
    public:
        static void Draw();
    private:
        static void PrepareConstantVoltage();
        static void PrepareVariableVoltage();
        static void PrepareConstantCurrent();
        static void PrepareVariableCurrent();
        static void PrepareResistance();
        static void PrepareTestDiode();
        static void PrepareRing();
    };
    /// �������������
    static void Init();

    static void Update();
    /// �������� �����������, ��� ��������� ��������� ������ � ����� ��������� ���������� ��������� ����� �������
    static void ChangeMode();

    static void ChangeAVP();
    /// ����� ��� ������� ��������� ��������� �� �������
    static void SetMeasure(const uint8 buffer[10]);

    static UART_HandleTypeDef handlerUART;

    /// ����� ��������� �����������
    struct Measure
    {
        enum E
        {
            VoltageDC,
            VoltageAC,
            CurrentDC,
            CurrentAC,
            Resistance,
            TestDiode,
            Bell,
            Number
        } value;
        Measure(E v) : value(v) { };
        operator uint8() const { return (uint8)value; };
        char Symbol() const
        {
            static const char symbols[Number] = {'U', 'V', 'I', 'J', 'R', 'Y', 'W' };
            return symbols[value]; //-V2006
        }
        static Measure ForSymbol(char symbol)
        {
			Measure result = Measure::Number;

            switch(symbol)
            {
				case 'V':
					result = Measure::VoltageAC;
					break;
				case 'I':
					result = Measure::CurrentDC;
					break;
				case 'J':
					result = Measure::CurrentAC;
					break;
				case 'R':
					result = Measure::Resistance;
					break;
				case 'Y':
					result = Measure::TestDiode;
					break;
				case 'W':
					result = Measure::Bell;
					break;
            }

            return result;
        }
    };

    /// ��� - ��������� �������
    struct AVP
    {
        enum E
        {
            Off,
            On
        } value;
        AVP(E v) : value(v) { };
        operator uint8() const { return (uint8)value; };
    };

    /// ������ ��������� ����������� ����������
    struct RangeDC
    {
        enum E
        {
            _2V,
            _20V,
            _500V
        } value;
        RangeDC(E v) : value(v) { };
        operator uint8() const { return (uint8)value; };
    };

    /// ������ ��������� ����������� ����������
    struct RangeAC
    {
        enum E
        {
            _2V,
            _20V,
            _400V
        } value;
    };

    /// ������ ��������� ������������ ����������� ����
    struct RangeResistance
    {
        enum E
        {
            _2k,
            _20k,
            _200k,
            _10M
        } value;
    };

private:
    /// ���� ������� ������� == 0, �� �������� ������ �� �����
    static char         buffer[11];
};
