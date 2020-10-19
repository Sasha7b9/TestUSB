#pragma once
#include "Multimeter/Multimeter.h"


#define S_MULT_SHOW_ALWAYS  (set.mult._showAlways)


struct SettingsMultimeter
{
    ModeAVP::E              _avp;
    MultimeterMeasure::E    _meas;
    RangeVoltageDC::E       _rangeVoltageDC;
    RangeVoltageAC::E       _rangeVoltageAC;
    RangeCurrentAC::E       _rangeCurrentAC;     // ������ ��������� ����������� ����
    RangeCurrentDC::E       _rangeCurrentDC;     // ������ ��������� ����������� ����
    RangeResistance::E      _rangeResist;
    bool                    _showAlways;         // true - ���������� � ������ ������������ ����
};



struct PageMultimeter
{
    static void Init();

    static void OnChanged_Mode(bool = true);

    // ���������� true, ���� "����" - "���"
    static bool ZeroEnabled();

    static void EnableZero(bool enable);
    
    // ���� ��������� ������� ���������� ��� �������� � ���� ����������
    static void DecodePassword(const KeyEvent &event);
    static void EnablePageCalibrate();
    static void DisablePageCalibrate();

    static const Page *const self;


    struct Calibration
    {
        static const Page *const self;
    };
};
