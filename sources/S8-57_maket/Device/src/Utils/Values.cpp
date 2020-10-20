#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "Values.h"
#include "Math.h"
#include "Settings/Settings.h"
#include <cstring>
#include <cmath>
#include <stdlib.h>
#include <limits>
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ����������� �������� ������� � ��������� ������
static char *FrequencyToString(float freq, char buffer[20]);
/// ����������� ����� � ��������� ������
static char *TimeToString(float time, bool alwaysSign, char buffer[20]);
/// ����������� �������� ���������� � ��������� ������
static char *VoltageToString(float voltts, bool alwaysSign, char buffer[20]);
/// \brief ��������� ����� � ��������� ������ � ��������� ���. numDigits - ����� �������� ���������
/// \attention ������ ����� ��������� �� ���������� ������ �������. ���� ��������� ����� ������� ���������� �������, �� ��� ����� ������������ ����.
/// \retval ��������� �� ������ � ������.
static char *FloatToString(float value, bool alwaysSign, int numDigits, char bufferOut[20]);
/// \brief ����������� value � ��������� ������
/// \attention ������ ����� ��������� �� ���������� ������ �������. ���� ��������� ����� ������� ���������� �������, �� ��� ����� ����������� ����
static char *IntegerToString(int value, bool alwaysSign, int numMinFields, char bufferOut[20]);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Float::Float(float val) : value(val)
{
    if (value == std::numeric_limits<float>::infinity())
    {
        LOG_ERROR("������������ �����");
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
char* Hex::ToBin(int depth, char buffer[36]) const
{
    int byte = 3;       /// � ����� ����� �������� �����. �.�. � ������ ������ - ������� ����, � ����� - �������

    switch (depth)
    {
        case 8: byte = 0;  break;
        case 16: byte = 1; break;
    }

    char *pointer = buffer;

    while (byte >= 0)
    {
        BinToString8((uint8)(value >> (byte * 8)), pointer);
        if (byte > 0)
        {
            *(pointer + 8) = ' ';
        }
        pointer += 9;
        byte--;
    }

    return buffer;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
char* Hex::BinToString8(uint8 val, char buffer[9]) const
{
    for (int bit = 0; bit < 8; bit++)
    {
        buffer[7 - bit] = _GET_BIT(val, bit) ? '1' : '0';
    }
    buffer[8] = '\0';
    return buffer;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
char* Hex::ToHex(int depth, char buffer[9]) const
{
    switch (depth)
    {
        case 8:     sprintf(buffer, "%02X", value); break;
        case 16:    sprintf(buffer, "%04X", value); break;
        case 32:    sprintf(buffer, "%08X", value); break;
        default:
            // ��� ������ �� ������
            break;
    }

    return buffer;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
char Hex::DigitInPosition(int pos) const
{
    uint val = value;

    while (pos > 0)
    {
        val /= 10;
        pos--;
    }

    return (char)((val % 10) | 0x30);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static char *IntegerToString(int value, bool alwaysSign, int numMinFields, char buffer[20])
{
    const int SIZE = 20;
    char format[SIZE] = "%";
    snprintf(&(format[1]), SIZE, "0%d", numMinFields);
    std::strcat(format, "d"); //-V2513

    if (alwaysSign && value >= 0)
    {
        buffer[0] = '+';
        snprintf(buffer + 1, SIZE - 1, format, value);
    }
    else
    {
        snprintf(buffer, SIZE, format, value);
    }

    return buffer;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static char *FrequencyToString(float freq, char bufferOut[20]) //-V2506
{
    bufferOut[0] = 0;
    const char *suffix = 0;

    if (Math::IsEquals(freq, ERROR_VALUE_FLOAT))
    {
        std::strcat(bufferOut, ERROR_STRING_VALUE); //-V2513
        return bufferOut;
    }

    if (freq >= 1e6F)
    {
        suffix = LANG_RU ? "���" : "MHz";
        freq /= 1e6F;
    }
    else if (freq >= 1e3F)
    {
        suffix = LANG_RU ? "���" : "kHz";
        freq /= 1e3F;
    }
    else
    {
        suffix = LANG_RU ? "��" : "Hz";
    }

    sprintf(bufferOut, "%s%s", Float(freq).ToString(false, 4).CString(), suffix);
    return bufferOut;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
char *Frequency::ToStringAccuracy(char bufferOut[20], int numDigits) const //-V2506
{
    float freq = value;

    bufferOut[0] = 0;
    const char *suffix = LANG_RU ? "��" : "Hz";

    if (Math::IsEquals(freq, ERROR_VALUE_FLOAT))
    {
        std::strcat(bufferOut, ERROR_STRING_VALUE); //-V2513
        return bufferOut;
    }

    if (freq >= 1e6F)
    {
        suffix = LANG_RU ? "���" : "MHz";
        freq /= 1e6F;
    }
    else if (freq >= 1e3F)
    {
        suffix = LANG_RU ? "���" : "kHz";
        freq /= 1e3F;
    }
    else
    {
        // ��� ������ �� ������
    }

    std::strcat(bufferOut, Float(freq).ToString(false, numDigits).CString()); //-V2513
    std::strcat(bufferOut, suffix); //-V2513
    return bufferOut;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String Time::ToString(bool alwaysSign) const
{
    char buffer[20];
    return String(TimeToString(value, alwaysSign, buffer));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String Frequency::ToString() const
{
    char buffer[50];
    return String(FrequencyToString(value, buffer));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
char *TimeToString(float time, bool alwaysSign, char buffer[20]) //-V2506
{
    if (Math::IsEquals(time, ERROR_VALUE_FLOAT))
    {
        std::strcpy(buffer, ERROR_STRING_VALUE); //-V2513
        return buffer;
    }

    pString suffix[2][4] =
    {
        {"��", "���", "��", "�"},
        {"ns", "us",  "ms", "s"}
    };

    static const float factor[4] = { 1e9F, 1e6F, 1e3F, 1.0F };

    float absTime = std::fabsf(time);

    int num = 0;

    if (absTime + 0.5e-10F < 1e-6F)
    {
    }
    else if (absTime + 0.5e-7F < 1e-3F)
    {
        num = 1;
    }
    else if (absTime + 0.5e-3F < 1.0F)
    {
        num = 2;
    }
    else
    {
        num = 3;
    }

    std::strcpy(buffer, Float(time * factor[num]).ToString(alwaysSign, 4).CString()); //-V2513
    std::strcat(buffer, suffix[LANG][num]); //-V2513
    return buffer;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
char* Time::ToStringAccuracy(bool alwaysSign, char buffer[20], int numDigits) const //-V2506
{
    float time = value;

    buffer[0] = 0;
    const char *suffix = LANG_RU ? "�" : "s";

    float fabsTime = std::fabsf(time);

    if (Math::IsEquals(time, ERROR_VALUE_FLOAT))
    {
        std::strcat(buffer, ERROR_STRING_VALUE); //-V2513
        return buffer;
    }
    else if (fabsTime + 0.5e-10F < 1e-6F)
    {
        suffix = LANG_RU ? "��" : "ns";
        time *= 1e9F;
    }
    else if (fabsTime + 0.5e-7F < 1e-3F)
    {
        suffix = LANG_RU ? "���" : "us";
        time *= 1e6F;
    }
    else if (fabsTime + 0.5e-3F < 1.0F)
    {
        suffix = LANG_RU ? "��" : "ms";
        time *= 1e3F;
    }
    else
    {
        // ��� ������ �� ������
    }

    std::strcat(buffer, Float(time).ToString(alwaysSign, numDigits).CString()); //-V2513
    std::strcat(buffer, suffix); //-V2513

    return buffer;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String Voltage::ToString(bool alwaysSign) const
{
    char buffer[20];
    return String(VoltageToString(value, alwaysSign, buffer));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static char* VoltageToString(float voltage, bool alwaysSign, char buffer[20]) //-V2506
{
    if (Math::IsEquals(voltage, ERROR_VALUE_FLOAT))
    {
        std::strcpy(buffer, ERROR_STRING_VALUE); //-V2513
        return buffer;
    }

    pString suf[2][4] =
    {
        {"\x10���", "\x10��", "\x10�", "\x10��"},
        {"\x10uV",  "\x10mV", "\x10V", "\x10kV"}
    };

    static const float factor[4] = { 1e6F, 1e3F, 1.0F, 1e-3F };

    int num = 0;
    float absValue = std::fabsf(voltage) + 0.5e-4F;

    if (absValue < 1e-3F)
    {
        num = 0;
    }
    else if (absValue < 1.0F)
    {
        num = 1;
    }
    else if (absValue < 1e3F)
    {
        num = 2;
    }
    else
    {
        num = 3;
    }

    std::strcpy(buffer, Float(voltage * factor[num]).ToString(alwaysSign, 4).CString()); //-V2513
    std::strcat(buffer, suf[LANG][num]); //-V2513
    return buffer;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String Current::ToString() const
{
    char buffer[50];
    return String(ToString(buffer));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
char *Current::ToString(char buffer[50]) const //-V2506
{
    float current = value;

    if (Math::IsEquals(current, ERROR_VALUE_FLOAT))
    {
        std::strcpy(buffer, ERROR_STRING_VALUE); //-V2513
        return buffer;
    }

    pString suf[2][4] =
    {
            { "\x10���", "\x10��", "\x10�", "\x10��" },
        { "\x10u�",  "\x10m�", "\x10�", "\x10k�"}
    };

    static const float factor[4] = { 1e6F, 1e3F, 1.0F, 1e-3F };

    int num = 0;

    float absValue = std::fabsf(current) + 0.5e-4F;

    if (absValue < 1e-3F)
    {
        num = 0;
    }
    else if (absValue < 1.0F)
    {
        num = 1;
    }
    else if (absValue < 1e3F)
    {
        num = 2;
    }
    else
    {
        num = 3;
    }

    std::strcpy(buffer, Float(current * factor[num]).ToString(true, 4).CString()); //-V2513
    std::strcat(buffer, suf[LANG][num]); //-V2513
    return buffer;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
char* Phase::ToString(char bufferOut[20]) const
{
    sprintf(bufferOut, "%s\xa8", Float(value).ToString(false, 4).CString());
    return bufferOut;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String Float::ToString(bool alwaysSign, int numDigits) const
{
    char buffer[30];
    return String(FloatToString(value, alwaysSign, numDigits, buffer));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String Integer::ToString(bool alwaysSign, int numMinFields) const
{
    char buffer[30];
    return String(IntegerToString(value, alwaysSign, numMinFields, buffer));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static char *FloatToString(float value, bool alwaysSign, int numDigits, char bufferOut[20]) //-V2506
{
    if (Math::IsEquals(value, ERROR_VALUE_FLOAT))
    {
        std::strcpy(bufferOut, ERROR_STRING_VALUE); //-V2513
        return bufferOut;
    }
    
    value = Math::RoundFloat(value, numDigits);
    
    char *pBuffer = bufferOut;
    
    if (value < 0)
    {
        *pBuffer++ = '-';
    }
    else if (alwaysSign)
    {
        *pBuffer++ = '+';
    }
    else
    {
        // ����� ������ �� ������
    }
    
    char format[10] = "%4.2f\0\0";
    
    format[1] = (char)numDigits + 0x30;
    
    int numDigitsInInt = Math::DigitsInIntPart(value);
    
    format[3] = (char)((numDigits - numDigitsInInt) + 0x30);
    if (numDigits == numDigitsInInt)
    {
        format[5] = '.';
    }
    
    float absValue = std::fabsf(value);
    sprintf(pBuffer, (char *)format, (double)absValue);
    
    float val = (float)atof(pBuffer); //-V2508
    
    if (Math::DigitsInIntPart(val) != numDigitsInInt)
    {
        numDigitsInInt = Math::DigitsInIntPart(val);
        format[3] = (char)((numDigits - numDigitsInInt) + 0x30);
        if (numDigits == numDigitsInInt)
        {
            format[5] = '.';
        }
        sprintf(pBuffer, format, (double)value);
    }
    
    bool signExist = alwaysSign || value < 0;
    while (std::strlen(bufferOut) < (size_t)(numDigits + (signExist ? 2 : 1))) //-V2513
    {
        std::strcat(bufferOut, "0"); //-V2513
    }
    
    return bufferOut;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int Hex::NumDigits() const
{
    uint val = value;

    int num = 1;
    while ((val /= 10) > 0)
    {
        num++;
    }
    return num;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int Integer::NumDigits() const
{
    int val = Math::Abs(value);
    int num = 1;
    while ((val /= 10) > 0)
    {
        num++;
    }
    return num;
}
