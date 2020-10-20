#pragma once
#include "Utils/Stack.h"


#define FREQ_METER_ENABLED                  (set.freq_enabled)
#define FREQ_METER_IS_ENABLED               (FREQ_METER_ENABLED == FrequencyCounter::Enabled::On)
#define FREQ_METER_MODE_VIEW                (set.freq_modeView)
#define FREQ_METER_MODE_VIEW_IS_FREQUENCY   (FREQ_METER_MODE_VIEW == FrequencyCounter::ModeView::Frequency)
#define FREQ_METER_MODE_VIEW_IS_PERIOD      (FREQ_METER_MODE_VIEW == FrequencyCounter::ModeView::Period)
#define FREQ_METER_TIMECOUNTING             (set.freq_timeCounting)
#define FREQ_METER_FREQ_CLC                 (set.freq_freqClc)
#define FREQ_METER_NUM_PERIODS              (set.freq_numberPeriods)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FrequencyCounter
{
public:
    /// �������������.
    static void Init();
    /// ������� ��������� ��� �������� �������
    static void LoadFreqSettings();
    /// ������� �������� ��� �������� �������
    static void LoadPeriodSettings();

    static void Update();
    /// ���������� ���������� �������� �������
    static float GetFreq();
    /// ��������� ��������
    static void Draw();

    /// ����� ����� �������.
    struct TimeCounting
    {
        enum E
        {
            _100ms,
            _1s,
            _10s
        } value;
        explicit TimeCounting(E v) : value(v) {};
    };

    /// ������� ����������� ��������� ��� ����� �������.
    struct FreqClc
    {
        enum E
        {
            _100kHz,
            _1MHz,
            _10MHz,
            _100MHz,
            Number
        } value;
        explicit FreqClc(E v) : value(v) {};
    };

    /// ���������� ��������.
    struct NumberPeriods
    {
        enum E
        {
            _1,
            _10,
            _100,
            _1k,
            _10k,
            _100k,
            Number
        } value;
        explicit NumberPeriods(E v) : value(v) {};
    };

    struct Enabled
    {
        enum E
        {
            Off,
            On
        } value;
        explicit Enabled(E v) : value(v) {};
    };

    /// ��� ���������� - ������ ��� �������
    struct ModeView
    {
        enum E
        {
            Frequency,
            Period,
            Number
        } value;
        explicit ModeView(E v) : value(v) {};
    };

private:
    /// ���������� ��������� �������� ��������� � ���������, ��������������� �������� �������
    static void SetStateLamps();

    static void SetStateLampFreq();

    static void SetStateLampPeriod();

    static void ReadFreq();

    static void ReadPeriod();

    static void LoadSettings();

    static float FreqSetToFreq(const BitSet32 *fr);

    static float PeriodSetToFreq(const BitSet32 *period);
    /// ������������� � true �������� ��������, ��� ������� ����� ������� �� �������� �������
    static bool readPeriod;

    static float prevFreq;

    static float frequency;

    static float period;
};
