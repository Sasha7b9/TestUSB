#include "stdafx.h"
#ifndef WIN32
#include <stm32f4xx.h>
#include "defines.h"
#include "log.h"
#include "FrequencyCounter.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Display/Painter.h"
#include "Utils/StringUtils.h"
#include "Utils/Values.h"
#include "Hardware/FSMC.h"
#include "Menu/Pages/Include/PageFunction.h"
#include "Utils/Math.h"
#include "Hardware/Timer.h"
#include <cstring>
#include <limits>
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ����� �������� ��������� �������������� �������� �������. ��� ������ � ������ �����������. 0 ��������, ��� �������� �������� �� ����
static BitSet32 freqActual;
/// ����� �������� ��������� �������������� �������� �������. ��� ������ � ������ �����������. 0 ��������, ��� �������� �������� �� ����
static BitSet32 periodActual;

/// ��� �������
/// \todo �������
static BitSet32 lastFreq;
static BitSet32 lastPeriod;
/// ��������� ����� ����������� ������ �������
static uint lastFreqRead;
/// ��������� ����� ���������� ������ �������
static uint lastPeriodRead;
/// ��������� ����� ������������ �������
static uint lastFreqOver;
/// ��������� ����� ����������� �������
static uint lastPeriodOver;
/// ���� true - ����� �������� ����� �������
static bool lampFreq = false;
/// ���� false - ����� �������� ����� �������
static bool lampPeriod = false;;


bool     FrequencyCounter::readPeriod;
float    FrequencyCounter::prevFreq;
float    FrequencyCounter::frequency;

//                         0    1    2    3    4    5    6 
static char buffer[11] = {'0', '0', '0', '0', '0', '0', '0', 0, 0, 0, 0};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ������� ���������� ����������
static void DrawDebugInfo();

static pString FreqSetToString(const BitSet32 *fr);

static pString PeriodSetToString(const BitSet32 *pr);
/// ���������� ������� �������� ������� ���������� �������� �������� ������� ��� ������ ����������
static int LowOrder(FrequencyCounter::FreqClc::E freqCLC, FrequencyCounter::NumberPeriods::E numPeriods);
/// ����������� 6 �������� �����, ���������� � �����, � ��������� ������ �������. ������� �������� ������ �������� �� ������� �����. order - ��� �������
static pString StackToString(Stack<uint> *stack, int order);
/// ���������� 6 �������� �� ����� stack � ����� buffer. ������� ������ �� ������� �����. ����� ������� �� point �������, ������� � buffer[0]
static void WriteStackToBuffer(Stack<uint> *stack, int point, const char *suffix);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FrequencyCounter::Init()
{
    LoadSettings();
    FSMC::WriteToFPGA8(WR_RESET_COUNTER_FREQ, 1);
    FSMC::WriteToFPGA8(WR_RESET_COUNTER_PERIOD, 1);
    freqActual.word = periodActual.word = 0;
}


#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4310)   // cast truncates constant value
#endif


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FrequencyCounter::LoadSettings()
{
    uint8 data = 0;

    if (FREQ_METER_IS_ENABLED)
    {
        const uint16 maskTime[3] = {0, 1, 2};
        const uint16 maskFreqClc[4] = {0, (1 << 2), (1 << 3), ((1 << 3) + (1 << 2))};

        DEF_STRUCT(StructPeriod, uint16) maskPeriod[NumberPeriods::Number] =
        { //        654 - ��������������� ����
            BIN_U8(00000000), //-V2501
            BIN_U8(00010000), //-V2501
            BIN_U8(00100000), //-V2501
            BIN_U8(00110000), //-V2501
            BIN_U8(01000000), //-V2501 // -V536
            BIN_U8(01010000)  //-V2501
        };

        data |= maskTime[FREQ_METER_TIMECOUNTING];
        data |= maskFreqClc[FREQ_METER_FREQ_CLC];
        data |= maskPeriod[FREQ_METER_NUM_PERIODS].val;
    }
    else
    {
        _SET_BIT(data, 2);
    }

    FSMC::WriteToFPGA8(WR_FREQMETER, data);
}


#ifdef WIN32
#pragma warning(pop)
#endif


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FrequencyCounter::LoadFreqSettings()
{
    LoadSettings();
    FSMC::WriteToFPGA8(WR_RESET_COUNTER_FREQ, 1);
    freqActual.word = 0;
    lampFreq = false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FrequencyCounter::LoadPeriodSettings()
{
    LoadSettings();
    FSMC::WriteToFPGA8(WR_RESET_COUNTER_PERIOD, 1);
    periodActual.word = 0;
    lampPeriod = false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FrequencyCounter::Update()
{
    SetStateLamps();

    bool freqReady = FPGA::GetFlag::FREQ_READY();

    if(freqReady)
    {
        lastFreqRead = TIME_MS;
    }

    bool periodReady = FPGA::GetFlag::PERIOD_READY();

    if(periodReady)
    {
        lastPeriodRead = TIME_MS;
    }

    if (freqReady)
    {
        freqActual.Set(*RD_FREQ_BYTE_3, *RD_FREQ_BYTE_2, *RD_FREQ_BYTE_1, *RD_FREQ_BYTE_0);

        lastFreq.Set(freqActual.word);
        
        if (!readPeriod)
        {
            ReadFreq();
        }
    }

    if (periodReady)
    {
        periodActual.Set(*RD_PERIOD_BYTE_3, *RD_PERIOD_BYTE_2, *RD_PERIOD_BYTE_1, *RD_PERIOD_BYTE_0);

        lastPeriod.Set(periodActual.word);

        if (readPeriod)
        {
            ReadPeriod();
        }
    }

    if(FPGA::GetFlag::FREQ_OVERFLOW())
    {
        freqActual.word = MAX_UINT;
        lastFreqOver = TIME_MS;
    }
    if(FPGA::GetFlag::PERIOD_OVERFLOW())
    {
        periodActual.word = MAX_UINT;
        lastPeriodOver = TIME_MS;
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FrequencyCounter::ReadFreq()
{
    BitSet32 freqSet(*RD_FREQ_BYTE_3, *RD_FREQ_BYTE_2, *RD_FREQ_BYTE_1, *RD_FREQ_BYTE_0);

    lastFreq.Set(freqSet.word);

    if (freqSet.word < 1000)
    {
        readPeriod = true;
    }
    else
    {
        float fr = FreqSetToFreq(&freqSet);
        if (fr < prevFreq * 0.9F || fr > prevFreq * 1.1F)
        {
            frequency = ERROR_VALUE_FLOAT;
        }
        else
        {
            frequency = fr;
        }
        prevFreq = fr;
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FrequencyCounter::ReadPeriod()
{
    BitSet32 periodSet(*RD_PERIOD_BYTE_3, *RD_PERIOD_BYTE_2, *RD_PERIOD_BYTE_1, *RD_PERIOD_BYTE_0);

    lastPeriod.Set(periodSet.word);

    float fr = PeriodSetToFreq(&periodSet);
    if (fr < prevFreq * 0.9F || fr > prevFreq * 1.1F)
    {
        frequency = ERROR_VALUE_FLOAT;
    }
    else
    {
        frequency = fr;
    }
    prevFreq = fr;
    readPeriod = false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
float FrequencyCounter::FreqSetToFreq(const BitSet32 *fr)
{
    const float k[3] = {10.0F, 1.0F, 0.1F};
    return FREQ_METER_IS_ENABLED ? (fr->word * k[FREQ_METER_TIMECOUNTING]) : (fr->word * 10.0F);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
float FrequencyCounter::PeriodSetToFreq(const BitSet32 *period_)
{
    if (period_->word == 0)
    {
        return 0.0F;
    }

    const float k[4] = {10e4F, 10e5F, 10e6F, 10e7F};
    const float kP[3] = {1.0F, 10.0F, 100.0F};

    return FREQ_METER_IS_ENABLED ? (k[FREQ_METER_FREQ_CLC] * kP[FREQ_METER_NUM_PERIODS] / (float)period_->word) : (10e5F / (float)period_->word);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
float FrequencyCounter::GetFreq()
{
    return frequency;
}

#define SIZE 4

#define EMPTY_STRING    "\xa9\xa9\xa9.\xa9\xa9\xa9"
#define OVERFLOW_STRING ">>>"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawFrequency(int x, int y)
{
    Painter::DrawBigText(x + 2, y + 1, SIZE, "F", Color::FILL);
    Painter::DrawBigText(x + 2, y + 10 * SIZE, SIZE, "T");

    Painter::DrawRectangle(x - 20, y, 10, 10);
    if (lampFreq)
    {
        Painter::FillRegion(x - 20, y, 10, 10);
    }

    int dX = 7 * SIZE;

    Painter::DrawBigText(x + dX, y + 1, SIZE, "=");
    Painter::DrawBigText(x + dX, y + 10 * SIZE, SIZE, "=");

    dX = SIZE * 12;

    char strFreq[50];
    std::strcpy(strFreq, FreqSetToString(&freqActual));

    Painter::DrawBigText(x + dX, y + 1, SIZE, strFreq);

    if (std::strcmp(strFreq, EMPTY_STRING) == 0)
    {
        return;
    }

    if (std::strcmp(strFreq, OVERFLOW_STRING) == 0)
    {
        return;
    }

    float freq = SU::StringToFloat(strFreq);

    if (std::strcmp(&strFreq[std::strlen(strFreq) - 3], "���") == 0)
    {
        freq *= 1e6F;
    }
    else if (std::strcmp(&strFreq[std::strlen(strFreq) - 3], "���") == 0)
    {
        freq *= 1e3F;
    }
    else if (std::strcmp(&strFreq[std::strlen(strFreq) - 3], "���") == 0)
    {
        freq *= 1e-3F;
    }
    else
    {
        // ���� ������� �� �����
    }

    Time time(1.0F / freq);

    Painter::DrawBigText(x + dX, y + 10 * SIZE, SIZE, time.ToStringAccuracy(false, strFreq, 6));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawPeriod(int x, int y)
{
    Painter::DrawBigText(x + 2, y + 1, SIZE, "T", Color::FILL);
    Painter::DrawBigText(x + 2, y + 10 * SIZE, SIZE, "F");

    Painter::DrawRectangle(x - 20, y + 1, 10, 10);
    if (lampPeriod)
    {
        Painter::FillRegion(x - 20, y, 10, 10);
    }

    int dX = 7 * SIZE;

    Painter::DrawBigText(x + dX, y + 1, SIZE, "=");
    Painter::DrawBigText(x + dX, y + 10 * SIZE, SIZE, "=");

    dX = SIZE * 12;

    char strPeriod[50];
    std::strcpy(strPeriod, PeriodSetToString(&periodActual));

    Painter::DrawBigText(x + dX, y + 1, SIZE, strPeriod);

    if ((std::strcmp(strPeriod, EMPTY_STRING) == 0) || (std::strcmp(strPeriod, OVERFLOW_STRING) == 0))
    {
        return;
    }

    float period = SU::StringToFloat(strPeriod);

    if (std::strcmp(&strPeriod[std::strlen(strPeriod) - 2], "��") == 0)
    {
        period *= 1e-9F;
    }
    else if (std::strcmp(&strPeriod[std::strlen(strPeriod) - 3], "���") == 0)
    {
        period *= 1e-6F;
    }
    else if (std::strcmp(&strPeriod[std::strlen(strPeriod) - 2], "��") == 0)
    {
        period *= 1e-3F;
    }
    else
    {
        LOG_ERROR("����� �� ������� �� ������ ���������");
    }

    Frequency freq(1.0F / period);

    Painter::DrawBigText(x + dX, y + 10 * SIZE, SIZE, freq.ToStringAccuracy(strPeriod, 6));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FrequencyCounter::Draw()
{
    /// \todo � ���� ������ ����� ������� �� ��� �������, � � ��� �������, ��� ��� ������ ��������� ���

    if (!FREQ_METER_IS_ENABLED)
    {
        return;
    }

    int width = 241;
    int height = 74;
    int x = Grid::Left() + (Grid::Width() - width) / 2;
    int y = Grid::Top() + (Grid::Height() / 2 - height) / 2;

    
    Painter::FillRegion(x + 1,   y + 1, width - 2, height - 2, Color::BACK);
    Painter::DrawRectangle(x,    y,     width,     height,     Color::FILL);

    x += 2;
    y += 2;

    if (FREQ_METER_MODE_VIEW_IS_FREQUENCY)
    {

        DrawFrequency(x, y);
    }
    else
    {
        DrawPeriod(x, y);
    }
   
    if(false)
    {
        DrawDebugInfo();
    }
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
pString PeriodSetToString(const BitSet32 *pr)
{
    if (pr->word == 0)
    {
        return EMPTY_STRING;
    }
    else if (pr->word == MAX_UINT)
    {
        return OVERFLOW_STRING;
    }
    else
    {
        // ��� ������ ����������
    }

    Stack<uint> stack(20);

    uint _period = pr->word;

    while(_period > 0)
    {
        stack.Push(_period % 10);
        _period /= 10;
    }

    int order = LowOrder(FREQ_METER_FREQ_CLC, FREQ_METER_NUM_PERIODS);  // � ����� - ������� �������� ��������� �������

    while(stack.Size() < 6)
    {
        stack.Push(0U);                             // �������� ����������� ����
    }
                                                    // ������ � ����� ��� ������� �������, ������� (� �������� order - �� ��� �����)

    Stack<uint> stackResult(6);                     // ����� ����� ��������� ����� �������� �������. ��������� - �������

    while(stackResult.Size() < 6)
    {
        stackResult.Push(stack.Pop());
    }
                                                    // ������ � stackResult ��� ������� ��������� �����. �� ������� - �������.
                                                    // ������������� ��� �������.

    while(!stack.Empty())
    {
        stack.Pop();
        order++;
    }
                                                    // ������ � order �������� ������� �������� ��������� ������� ���������� (�� ��������� �� �������)

    return StackToString(&stackResult, order);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static int LowOrder(FrequencyCounter::FreqClc::E freqCLC, FrequencyCounter::NumberPeriods::E numPeriods)
{
/*
    ���������� �������� | ����������� �������� | ����� �� ����� | ��������� �������� ������

    +----------+-------------------------------------+-------------------------------------+-------------------------------------+-------------------------------------+
    |          | 100 ���                    0        | 1 ���                      1        | 10 ���                     2        | 100 ���                    3        |
    +----------+-------------------------------------+-------------------------------------+-------------------------------------+-------------------------------------+
    | 1        | 10   ���        1    0.01 ��  1e-5  | 1 ���          1      1. ���  1e-6  | 100 ��         1     0.1 ���  1e-7  | 10  ��         1    0.01 ���  1e-8  | 
    |          | 100  ���       10    0.10 ��  1e-5  | 10 ���        10     10. ���  1e-6  | 1   ���       10     1.0 ���  1e-7  | 100 ��        10    0.10 ���  1e-8  |
    |          | 1    ��       100    1.00 ��  1e-5  | 100 ���      100    100. ���  1e-6  | 10  ���      100    10.0 ���  1e-7  | 1   ���      100    1.00 ���  1e-8  |
    |          | 10   ��      1000   10.00 ��  1e-5  | 1   ��      1000   1.000 ��   1e-6  | 100 ���     1000   100.0 ���  1e-7  | 10  ���     1000   10.00 ���  1e-8  |
    |          | 100  ��     10000  100.00 ��  1e-5  | 10  ��     10000  10.000 ��   1e-6  | 1   ��     10000  1.0000 ��   1e-7  | 100 ���    10000  100.00 ���  1e-8  |
    |          | 1    �     100000 1.00000 �   1e-5  | 100 ��    100000 100.000 ��   1e-6  | 10  ��    100000 10.0000 ��   1e-7  | 1   ��    100000 1.00000 ��   1e-8  |
    |          | 10   �    100000- 10.0000 �   1e-4  | 1   �    100000- 1.00000 �    1e-5  | 100 ��   100000- 100.000 ��   1e-6  | 10  �c   100000- 10.0000 ��   1e-7  |
    |          | 100  �   100000-- 100.000 �   1e-3  | 10  �   100000-- 10.0000 �    1e-4  | 1   �   100000-- 1.00000 �    1e-5  | 100 ��  100000-- 100.000 ��   1e-6  |
    |        0 | 1000 �   ������������               | 100 �   ������������                | 10  �   ������������                | 1   c   ������������                |
    +----------+-------------------------------------+-------------------------------------|-------------------------------------+-------------------------------------+
    | 10       |                                     |                                     |                                     | 1   ��         1      1. ��   1e-9  |
    |          |                                     |                                     |                         /           | 10  ��        10     10. ��   1e-9  |
    |          |                                     |                                     |                     /               | 100 ��       100    100. ��   1e-9  |
    |          |                                     |                                     |                  /                  | 1   ���     1000   1.000 ���  1e-9  |
    |          |                                     |                                     |             /                       | 10  ���    10000  10.000 ���  1e-9  | 
    |          |                                     |                                     |         /                           | 100 ���   100000 100.000 ���  1e-9  | 
    |          |                                     |                                     |   |  /                              | 1   ��   100000- 1.00000 ��   1e-8  | 
    |          |                                     |                                     |   |---                              | 10  ��  100000-- 10.0000 ��   1e-7  |
    |        1 |                                     |                                     |                                     | 100 ��  ������������                |
    +----------+-------------------------------------+-------------------------------------+-------------------------------------+-------------------------------------+
    | 100      |                                     |                                     |                                     | 0.1 ��         1     0.1 ��   1e-10 |
    |          |                                     |                                     |                           /         | 1   ��        10     1.0 ��   1e-10 |
    |          |                                     |                                     |                       /             | 10  ��       100    10.0 ��   1e-10 |
    |          |                                     |                                     |                    /                | 100 ��      1000   100.0 ��   1e-10 |
    |          |                                     |                                     |               /                     | 1   ���    10000  1.0000 ���  1e-10 |
    |          |                                     |                                     |           /                         | 10  ���   100000 10.0000 ���  1e-10 |
    |          |                                     |                                     |     |  /                            | 100 ���  100000- 100.000 ���  1e-9  |
    |          |                                     |                                     |     |---                            | 1   ��  100000-- 1.00000 ��   1e-8  |
    |        2 |                                     |                                     |                                     | 10  ��  ������������                |
    +----------+-------------------------------------+-------------------------------------+-------------------------------------+-------------------------------------+
*/

    //                           | 100�/1 | 1�  /1  |  10� /1   | 100�/1   |            |           |           |           |
    //                           |        | 100�/10 |  1�  /10  | 10� /10  | 100�/10    |           |           |           |
    //                           |        |         |  100�/100 | 1�  /100 | 10� /100   | 100�/100  |           |           |
    //                           |        |         |           | 100�/1�  | 1�  /1�    | 10� /1�   | 100�/1�   |           |
    //                           |        |         |           |          | 100�/10�   | 1�  /10�  | 10� /10�  | 100�/10�  |
    //                           |        |         |           |          |            | 100�/100� | 1�  /100� | 10� /100� | 100�/100� |          |
    //                           |        |         |                                                 100�/          | 1�  /10�  | 10� /10�  | 100�/10� |
    //                                                                                                                                             | 100�/100�
    static const int order[] =   {  -5,      -6,        -7,        -8,         -9,         -10,        -11,        -12,        -13,      -14,       -15,        -16};

    return order[freqCLC + numPeriods];
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static pString StackToString(Stack<uint> *stack, int order)
{
    static const struct StructOrder
    {
        int first;
        int second;
        const char *s1;
        const char *s2;
    }
    structs[] =
    {
        {3, 6, "��", "��"},     // -12
        {1, 4, "���", "��"},    // -11
        {2, 5, "���", "��"},    // -10
        {3, 6, "���", "��"},    // -9
        {1, 4, "��", "���"},    // -8
        {2, 5, "��", "���"},    // -7
        {3, 6, "��", "���"},    // -6
        {1, 4, "�", "��"},      // -5
        {2, 5, "�", "��"},      // -4
        {3, 6, "�", "��"},      // -3
        {4, 4, "�", "�"},       // -2
        {5, 5, "�", "�"},       // -1
        {6, 6, "�", "�"}        // 0
    };

    order += 12;

    const StructOrder &str = structs[order];

    if (stack->NumFirstZeros() < str.first)
    {
        WriteStackToBuffer(stack, str.first, str.s1);
    }
    else
    {
        WriteStackToBuffer(stack, str.second, str.s2);
    }

    return buffer;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void WriteStackToBuffer(Stack<uint> *stack, int point, const char *suffix)
{
    for(int i = 6; i >= 0; i--)
    {
        if(point == i)
        {
            buffer[i] = '.';
            continue;
        }
        buffer[i] = ((char)stack->Pop() | 0x30);
    }

    std::strcpy(&buffer[7], suffix);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static pString FreqSetToString(const BitSet32 *fr)
{
    if(fr->word == 0)
    {
        return EMPTY_STRING;
    }
    else if(fr->word == MAX_UINT)
    {
        return OVERFLOW_STRING;
    }
    else
    {
        LOG_ERROR("������");
    }

    Hex value(fr->word);

    while(value.NumDigits() > 6)
    {
        value.Set(value / 10);
    }

    for(int i = 0; i < 7; i++)
    {
        buffer[i] = value.DigitInPosition(6 - i);
    }

    uint freq = fr->word;

    uint giverFreq = freq;

/// ��� ����� * 10
#define _10Hz   (               100) /* E_2 */
#define _100Hz  (              1000) /* E_3 */
#define _1kHz   (         10 * 1000) /* E_4 */
#define _10kHz  (        100 * 1000) /* E_5 */
#define _100kHz (       1000 * 1000) /* E_6 */
#define _1MHz   (  10 * 1000 * 1000) /* E_7 */
#define _10MHz  ( 100 * 1000 * 1000) /* E_8 */
#define _100MHz (1000 * 1000 * 1000) /* E_9 */


#undef WRITE_SUFFIX
#define WRITE_SUFFIX(suffix_E4)    \
    if(giverFreq < _1kHz) { std::strcpy(buffer + 7, suffix_E4); } else if (giverFreq < _1MHz) { std::strcpy(buffer + 7, "���"); } else { std::strcpy(buffer + 7, "���"); }

#define HIGH_FREQ                            \
    if(giverFreq < _10MHz)                   \
    {                                        \
        std::memmove(buffer, buffer + 1, 2); \
        buffer[1] = '.';                     \
    }                                        \
    else if (giverFreq < _100MHz)            \
    {                                        \
        std::memmove(buffer, buffer + 1, 3); \
        buffer[2] = '.';                     \
    }                                        \
    else                                     \
    {                                        \
        std::memmove(buffer, buffer + 1, 3); \
        buffer[3] = '.';                     \
    }


    switch (FREQ_METER_TIMECOUNTING)
    {
        case FrequencyCounter::TimeCounting::_100ms:

            giverFreq *= 100;

            WRITE_SUFFIX("���");

            if(giverFreq < _1MHz)                       // ������ 1 ���
            {
                if(freq >= _10Hz)                       // ������ ��� ����� 10 ��
                {
                    std::memmove(buffer, buffer + 1, 5);
                }
                buffer[4] = '.';
            }
            else
            {
                HIGH_FREQ;
            }            
            break;

        case FrequencyCounter::TimeCounting::_1s:

            giverFreq *= 10;

            WRITE_SUFFIX("��");

            if (giverFreq < _1MHz)                      // ������ 1 ���
            {
                if(giverFreq < _1kHz)                   // ������ 1 ���
                {
                    std::memmove(buffer, buffer + 1, 6);
                    buffer[6] = '.';
                }
                else
                {
                    std::memmove(buffer, buffer + 1, 4);
                    buffer[3] = '.';
                }
            }
            else
            {
                HIGH_FREQ;
            }
            break;

        case FrequencyCounter::TimeCounting::_10s:

            WRITE_SUFFIX("��");

            if (freq < _1MHz)                       // ������ 1 ���
            {
                if (giverFreq < _1kHz)              // ������ 1 ���
                {
                    std::memmove(buffer, buffer + 1, 5);
                    buffer[5] = '.';
                }
                else if(giverFreq < _100kHz)
                {
                    std::memmove(buffer, buffer + 1, 3);
                    buffer[2] = '.';
                }
                else
                {
                    std::memmove(buffer, buffer + 1, 3);
                    buffer[3] = '.';
                }
            }
            else
            {
                HIGH_FREQ;
            }
            break;
        default:
            LOG_ERROR("");
            break;
    }

    return buffer;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FrequencyCounter::SetStateLamps()
{
    SetStateLampFreq();
    SetStateLampPeriod();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FrequencyCounter::SetStateLampFreq()
{
    if(!lampFreq)
    {
        if(FPGA::GetFlag::FREQ_IN_PROCESS())
        {
            lampFreq = true;
        }
    }
    else
    {
        if(FPGA::GetFlag::FREQ_READY())
        {
            lampFreq = false;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FrequencyCounter::SetStateLampPeriod()
{
    if(!lampPeriod)
    {
        if(FPGA::GetFlag::PERIOD_IN_PROCESS())
        {
            lampPeriod = true;
        }
    }
    else
    {
        if(FPGA::GetFlag::PERIOD_READY())
        {
            lampPeriod = false;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void DrawDebugInfo()
{
    int width = 50;
    int height = 27;
    int x = 50;
    int y = 120;
    Painter::FillRegion(x, y, width, height, Color::BACK);
    Painter::DrawRectangle(x - 1, y - 1, width + 2, height + 2, Color::FILL);

    String("%d", freqActual.word).Draw(x + 4, y + 4);
    String("%d", periodActual.word).Draw(x + 4, y + 15);

    x += 100;
    width = 120;


    Painter::FillRegion(x, y, width, height, Color::BACK);
    Painter::DrawRectangle(x - 1, y - 1, width + 2, height + 2, Color::FILL);

    String("%d", lastFreq.word).Draw(x + 4, y + 4);
    String("%d", lastPeriod.word).Draw(x + 4, y + 15);

    int size = 8;

    x += 60;

#define TIME 250

    Painter::DrawRectangle(x, y + 4, size, size, Color::FILL);

    if (TIME_MS - lastFreqRead < TIME)
    {
        Painter::FillRegion(x + 1, y + 5, size - 2, size - 2, Color::BLUE);
    }

    Painter::DrawRectangle(x, y + 15, size, size, Color::FILL);

    if (TIME_MS - lastPeriodRead < TIME)
    {
        Painter::FillRegion(x + 1, y + 16, size - 2, size - 2, Color::BLUE);
    }

    x += 20;

    Painter::DrawRectangle(x, y + 4, size, size, Color::FILL);

    if (TIME_MS - lastFreqOver < TIME)
    {
        Painter::FillRegion(x + 1, y + 5, size - 2, size - 2, Color::RED);
    }

    Painter::DrawRectangle(x, y + 15, size, size, Color::FILL);

    if (TIME_MS - lastPeriodOver < TIME)
    {
        Painter::FillRegion(x + 1, y + 16, size - 2, size - 2, Color::RED);
    }

    x += 20;

    if (FPGA::GetFlag::FREQ_IN_PROCESS())
    {
        Painter::FillRegion(x + 1, y + 5, size - 2, size - 2, Color::FILL);
    }

    if (FPGA::GetFlag::PERIOD_IN_PROCESS())
    {
        Painter::FillRegion(x + 1, y + 16, size - 2, size - 2, Color::FILL);
    }
}
