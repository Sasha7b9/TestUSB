#pragma once


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Word
{
    char   *address;
    int8    numSymbols;
    uint8   notUsed0;
    uint8   notUsed1;
    uint8   notUsed2;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SU
{
public:
    /// ��� ������� ����� ����������� � �������� �������� �����.
    static bool GetWord(const char *string, Word *word, const int numWord);

    static bool WordEqualZeroString(Word *word, char* string);

    static char *Db2String(float value, int numDigits, char bufferOut[20]);
    /// ���������� ��� ������. ����� �������� ������ �� str1
    static bool EqualsStrings(const char *str1, const char *str2);

    static bool EqualsStrings(void *str1, void *str2, uint size);

    static bool EqualsStrings(uint8 *str1, const char * const str2, uint size);

    static bool EqualsZeroStrings(char *str1, char *str2);
    /// ���������� ������� ������� �������, ������� �� �������� ������ ���� ������
    static int FirstNotNumeral(char *buffer);
    /// ����������� ������ � float. �������������� ������� �� ������� ����������� �������
    static float StringToFloat(char *string);

    static char *ToUpper(void *str, uint size);
};
