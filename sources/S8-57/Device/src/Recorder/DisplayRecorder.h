#pragma once
#include "Osci/ParametersOsci.h"


struct Record;


class DisplayRecorder
{
public:

    static void Update();

    // �������� ���� ��������� �����
    static void MoveWindowLeft();

    // �������� ���� ��������� ������
    static void MoveWindowRight();

    // ����������� ������� ������ �����
    static void MoveCursorLeft();

    // ����������� ������� ������ ������
    static void MoveCursorRight();

    // ���������� true, ���� ��� ��� ���������� ����������� ������
    static bool InProcessUpdate();

    // ���������� ������������ ������
    static void SetDisplayedRecord(Record *record, bool forListening);

    struct Cursor
    {
        enum E
        {
            _1,
            _2
        };
    };


    // �������� ����������� ���� �� ������ � ������ ���������
    struct SpeedWindow
    {
        enum E
        {
            Cell,           // 1 ������
            _1Window,       // 1 ����
            _10Windows      // 10 ����
        } value;

        SpeedWindow(E v) : value(v)  { }

        int NumPoints() const;      // ���������� ����� �����, �� ������� ����� �������� �����
    };

    static SpeedWindow speed;
};
