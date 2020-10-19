#pragma once
#include "Osci/ParametersOsci.h"


class IntRAM
{
public:
    // ��������� �� 16k ���� ������, ��������������� ��� ������ �����������
    static uint16 *Averager32k(Chan::E ch);

    // ������ ��� ������ ������ � ������ �������������
    static uint8 *DataRand(Chan::E ch);

    // ��������� �� ������ ����������� ������
    static DataSettings *PrepareForP2P();
};
