#pragma once
#include "Tester/Tester.h"


struct SettingsTester
{
    Tester::Control::E  _control;
    Tester::Polarity::E _polarity;       // ���������� ������������ �����������
    Tester::StepU::E    _stepU;          // ��������� ��� ��������� ����������
    Tester::StepI::E    _stepI;          // ��������� ��� ��������� ����
    ENumAverage::E      _enumAverage;    // ���������� ����������
    Tester::ViewMode::E _viewMode;       // ����� ����������� ������
};



struct PageTester
{
    static void Init();

    static void OnChanged_Control(bool = true);

    static const Page *const self;
};
