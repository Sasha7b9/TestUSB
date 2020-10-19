#pragma once


class Battery
{
public:

    // ���������� �������, ��� ������� ����� ����������� ���������� �������
    static const float SHUTDOWN_VOLTAGE;

    static void Init();

    static void Draw(int x, int y);

    // ���������� �������� ���������� �������
    static float GetVoltage();

    static float GetVoltageAverage();

    // ����� ��� HAL_TIM5 - ��� true ������ ���������� � ����������
    static bool IsBusy();
};
