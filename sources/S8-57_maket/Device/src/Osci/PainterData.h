#pragma once
#include "FPGA/FPGATypes.h"
#include "Data/Reader.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class PainterData
{
public:
    
    static void DrawData();

    static void DrawChannel(Chan::E ch);

private:
    /// ���������� ���������� ������ - ��������������� ������� ����������
    static void DrawCurrent();
    /// ���������� ������ �� ���
    static void DrawRAM();
    /// ���������� ������ �� ����
    static void DrawROM();

    static void DrawTPos(int leftX, int rightX);

    static void DrawTShift(int leftX, int rightX, int numPoints);
    /// ���������� (-1), ���� ����� �� ������� (NONE_VALUE)
    static int Ordinate(uint8 x, float scale);
    /// \brief ���������� ����� � �������� ����������. ���� ����� �� ������� (NONE_VALUE), ���������� -1.
    /// ��������� ������������ width ������ 255.
    static void SendToDisplayDataInRect(Chan::E chan, int x, const int *min, const int *max, int width);

    static StructDataDrawing *dataStruct;
};
