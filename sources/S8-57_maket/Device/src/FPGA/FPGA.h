#pragma once
#include "FPGATypes.h"
#include "Settings/SettingsService.h"
#include <stm32f4xx_hal.h>


/// �� ������� ������ ����� �������� �������� ��������, ����� ������ �������� �� ���������� ������������ �� ���� �����.
#define STEP_RSHIFT     (((RShift::MAX - RShift::MIN) / 24) / 20)
#define STEP_TRIGLEV    STEP_RSHIFT

struct Pin
{
    enum E
    {
        SPI3_SCK,
        SPI3_DAT,
        SPI3_CS1,
        SPI3_CS2,
        A1,
        A2,
        A3,
        A4,
        LF1,
        LF2,
        A1S,
        A0S,
        LFS,
        Number
    } value;
    explicit Pin(E v) : value(v) {};
};

#define FPGA_IN_STATE_STOP (FPGA::fpgaStateWork == StateWorkFPGA_Stop)
#define FPGA_IN_STATE_WORK (FPGA::fpgaStateWork == StateWorkFPGA_Work)
#define FPGA_IN_STATE_WAIT (FPGA::fpgaStateWork == StateWorkFPGA_Wait)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct HardwareFPGA
{
public:
    static void LoadTrigSource();
    /// ���������� � �������������� ��������� ������, ���������� �� �������� � ���� �������������
    static void LoadTrigSourceInput();

    static void LoadRanges();

    static void LoadRShift(Chan::E ch);

    static void LoadTrigLev();

    static void LoadTBase();

    static void LoadTShift();
    /// ��������/��������� ����������.
    static void LoadCalibratorMode();

private:
    static uint8 ValueForRange(Chan::E ch);
};

class FPGA
{
friend class TrigLev;
friend struct HardwareFPGA;
public:

    static HardwareFPGA hardware;

    static void Init();

    static void Update();

    static void Start();

    static void OnPressStart();

    static void GiveStart();

    static void LoadSettings();

    static void ChangeRange(Chan::E ch, int delta);

    static void TBaseChange(int delta);

    static void RShiftChange(Chan::E ch, int delta);

    static void TrigLevChange(int delta);

    static void TShiftChange(int delta);

    static void SetRShift(Chan::E ch, uint16 rShift);

    static void LoadTrigPolarity();

    static void LoadTrigMode();

    static void LoadTrigInput();

    static void Stop(bool pause = false);

    static uint NumPoints();

    static bool IsRunning() { return isRunning; }

    static bool InStateStop() { return false; }

    static StateWorkFPGA GetStateWork();

    static void SetTShift(int tShift);

    static void Reset();

    static void LoadRegUPR();
    /// ���������� �������� ��������� �������������
    static void LoadHoldfOff();

    static void SetModeCouple(Chan::E ch, ModeCouple::E couple);

    /// ���������� ������������� ������� �������������
    static void SetTrigLev(Trig::Source::E ch, uint16 trigLev);
    /// ���������� ���������� ����������� �������� � �������
    static void SetENumSignalsInSec(int numSigInSec);
    /// ���������� ���������� ���������, �� ������� ����� �������������� ������ � ������ �������������
    static void SetNumberMeasuresForGates(int number);
    /// ���������� ������� �� �������
    static void SetTBase(TBase tBase);
    /// ����� � ���������� ������� ������������� �� ���������� ���������� �������
    static void FindAndSetTrigLevel();

    static void EnableRecorderMode(bool enable);
    /// ���������� ���������� �������� �� ������� ��� ������ �������������. � ������ �������� ��� �������� ������ ���� ������
    static void SetDeltaTShift(int16 shift);

    static void SetBandwidth(Chan::E ch);

    static ADC_HandleTypeDef *HandleADC() { return &handleADC; };

    static void SetValueADC(uint16 value);
    /// ������� ����������
    static void DoCalibration();

private:

    static void GPIO_Init();

    static void WritePin(Pin::E pin, int enable);

    static void SetPin(Pin::E pin);

    static void ResetPin(Pin::E pin);

    static uint GetPin(Pin::E pin);

    static GPIO_TypeDef *GetPort(Pin::E pin);

    static void WriteRegisters(Pin::E cs, uint16 value);

    static void ReadData();

    static void ReadFlag();

    static uint16 ReadLastRecord();

    static void ReadDataChanenl(Chan::E ch, uint8 data[FPGA_MAX_NUM_POINTS]);
    /// ������ ����� � ������������� � ������ address
    static void ReadDataChanenlRand(Chan::E ch, const uint8 *address, uint8 *data);

    static bool CalculateGate(uint16 rand, uint16 *eMin, uint16 *eMax);

    static int CalculateShift();

    static bool isRunning;
    /// True, ���� ��� ������
    static bool givingStart;
    /// ����� ������ ������
    static uint timeStart;

    static uint16 flag;
    /// ������������ � ������ �������������
    static ADC_HandleTypeDef handleADC;
    /// ��������, ��������� �� handleADC
    static uint16 valueADC;
    
public:
    static StateWorkFPGA fpgaStateWork;

    static uint16 post;
    static uint16 pred;

private:

    static struct State
    {
        bool needCalibration;                       ///< ������������� � true �������� ��������, ��� ���������� ���������� ����������.
        StateWorkFPGA stateWorkBeforeCalibration;
        StateCalibration stateCalibration;          ///< ������� ��������� ����������. ������������ � �������� ����������.
    } state;

    struct Flag
    {
        enum E
        {
            _DATA_READY        = 0,  ///< ������ ������ ��� ���������� (��������� �������� ������������)
            _TRIG_READY        = 1,  ///< ���� ��������������
            _PRED              = 2,  ///< ���� 1, �� ���������� ��������, ����� ������ �������������� ������ (���������
            _P2P               = 3,  ///< 
            _FREQ_READY        = 4,  ///< ���� ���������� ��������� �������
            _PERIOD_READY      = 5,  ///< ���� ���������� ��������� �������
            _HOLD_OFF_FLAG     = 7,  ///< �������������� ������ ����� ������ ������ ����� ���� ���� � �������
            _FREQ_OVERFLOW     = 8,  ///< ������� ������������ �������� �������
            _PERIOD_OVERFLOW   = 9,  ///< ������� ������������ �������� �������
            _FREQ_IN_PROCESS   = 10, ///< ������������� � ������� �������� ��������, ��� ��� ������� ��������� - ������� ������� � �������
            _PERIOD_IN_PROCESS = 11  ///< ������������� � ������� �������� ��������, ��� ��� ������� ��������� - ������� ������� � �������
        } value;
    };

public:

    struct GetFlag
    {
        static bool DATA_READY();
        static bool TRIG_READY();
        static bool PRED();
        static bool P2P();
        static bool FREQ_READY();
        static bool HOLD_OFF();
        static bool PERIOD_READY();
        static bool FREQ_OVERFLOW();
        static bool PERIOD_OVERFLOW();
        static bool FREQ_IN_PROCESS();
        static bool PERIOD_IN_PROCESS();
    };

    class FreqMeter
    {
    public:
        static float GetFreq();
    };

    class ForTester
    {
        friend class Tester;
    private:
        static bool Read(uint8 *dataA, uint8 *dataB);
        /// ��������� ���� ������ ��� ������-����������. � ������� time ������ ������ ���� ������� numPoints �����
        static void Start();
    };
};
