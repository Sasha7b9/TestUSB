#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "Keyboard.h"
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Key::E Keyboard::WaitPressingButton()
{
    return Key::None;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Keyboard::Enable()
{
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Keyboard::Disable()
{
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
pString Key::Name()
{
    static const pString names[Key::Number] =
    {
        "None",
        "�������",
        "�����",
        "������",
        "������",
        "����� 1",
        "����� 2",
        "���������",
        "����/����",
        "�����",
        "�������",
        "Range 1 ������",
        "Range 1 ������",
        "RShift 1 ������",
        "RShift 1 ������",
        "Range 2 ������",
        "Range 2 ������",
        "RShift 2 ������",
        "RShift 2 ������",
        "TBase ������",
        "TBase ������",
        "TShift ������",
        "TShift ������",
        "����� ������",
        "����� ������",
        "�����",
        "������",
        "�����",
        "����",
        "����",
        "1",
        "2",
        "3",
        "4",
        "5"
    };

    return names[value];
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
pString TypePress::ToString()
{
    static const pString names[TypePress::Number] =
    {
        "����",
        "������",
        "�����",
        "�������",
        "�����"
    };

    return names[value];
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Key::IsControlSignal() const
{
    static const bool isControl[Key::Number] =
    {
        false, // None
        false, // Function
        false, // Measure
        false, // Memory
        false, // Service
        false, // ChannelA
        false, // ChannelB
        false, // Time
        false, // Start
        false, // Trig
        false, // Display
        true,  // RangeMoreA
        true,  // RangeLessA
        true,  // RShiftMoreA
        true,  // RShiftLessA
        true,  // RangeMoreB
        true,  // RangeLessB
        true,  // RShiftMoreB
        true,  // RShiftLessB
        false, // TBaseMore
        false, // TBaseLess
        false, // TShiftMore
        false, // TShiftLess
        false, // TrigLevMore
        false, // TrigLevLess
        false, // Left
        false, // Right
        false, // Up
        false, // Down
        false, // Enter
        false, // F1
        false, // F2
        false, // F3
        false, // F4
        false  // F5
    };


    return  isControl[value];
}
