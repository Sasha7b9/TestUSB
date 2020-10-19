#pragma once


struct Grid
{
    // ��������� ������������ ��� ������� ��������, ������� �������� ����������� ��� ����� �������� � ���������� ����� - ��������, ��������� ����
    static const int WIDTH = 280;
    static const int HEIGHT = 200;
    static const int SIZE_CELL = 20;

    static void Draw();

    // ��� �������� ����� �������� � ����������� �� ������������� ������
    static int Left();
    static int Top();
    static int Width();
    static int Height();
    static int Bottom();
    static int Right();
    static int ChannelCenterHeight();
    static int ChannelHeight();
    static int FullBottom();
    static int MathBottom();
    static int MathHeight();
    static int ChannelBottom();
    // ���������� ������ y-���������� ����, � ������� ����� �������� ��������������
    static int BottomForWarnings();
};
