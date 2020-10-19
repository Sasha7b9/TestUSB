#pragma once
#include "FreqMeter/FreqMeter.h"


class Page;
class Choice;


#define S_FREQ_METER_ENABLED    (set.freq._enabled)


struct SettingsFreqMeter
{
    bool                         _enabled;          // ������� �� ����� �����������
    FreqMeter::ModeMeasure::E    _modeMeasure;      // ����� ��������� - ������ ��� �������
    FreqMeter::TimeCounting::E   _timeCounting;     // ����� ����� �������.
    FreqMeter::FreqClc::E        _freqClc;          // ������� ����������.
    FreqMeter::NumberPeriods::E  _numberPeriods;    // ���������� ��������.
};



struct PageFreqMeter
{
    static const Page *const self;

    static void Init();
};
