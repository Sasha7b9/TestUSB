#pragma once
#include "Keyboard/Keyboard.h"


/*
    � ���� ����� - �������� ���� - ��� �������-������� �� ������� ������� ����������
*/


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ������� - ����������� ���������� �������
class TriggerDebugConsole
{
public:
    /// ���������� true, ���� ������� �������� - ������� ���������
    static bool Update(Key::E key);

private:
    static const int SIZE_BUFFER_FOR_BUTTONS = 10;
    static Key::E bufferForButtons[SIZE_BUFFER_FOR_BUTTONS];
    static const Key::E sampleBufferForButtons[SIZE_BUFFER_FOR_BUTTONS];
};
