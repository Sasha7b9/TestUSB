#pragma once
#include "Data/DataSettings.h"


/// ����� �������� ������ 
class DataBuffer
{
    friend class DataStorage;

public:
    static void Init();

    static uint Size();

private:
    /// �������� ������, ��������� � DataSettings, ���������� � �����. ���� ����� �� �������, �� ����� ������ ���������� ������ ���������
    static void Push(DataSettings *ds);
    /// ���������� ��������� �� ������, ���������� � ����� ����������
    static DataSettings *Top();

    /// ��������������� ����� ��� �������� ������ � �������� DataSettings
    class Stack
    {
    public:

        static uint Size();
        /// � ����������� ��������� ����� ������� �������� � ���������� ���������
        static void Clear();

        static void Push(DataSettings *ds);

        static DataSettings *Top();
    };
};
