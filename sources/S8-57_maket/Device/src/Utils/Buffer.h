#pragma once
#include "defines.h"


/*
    ����� ����������� ������������ ��� ��������������� ���������/������������ ������ �� ����
*/

class Buffer
{
public:
    Buffer(uint size);
    ~Buffer();
    uint8 *Data();
    uint   Size() const;
private:
    uint8 *buffer;
    uint size;
};
