#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "BottomPart.h"
#include "Display/Symbols.h"
#include "Settings/SettingsTime.h"
#include "FPGA/FPGAMath.h"
#include "Tables.h"
#include "Display/Painter.h"
#include "Utils/StringUtils.h"
#include "FlashDrive/FlashDrive.h"
#include "Utils/Dictionary.h"
#include "Utils/Values.h"
#include "Hardware/CPU.h"
#include "Hardware/VCP.h"
#include "Data/Reader.h"
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// �������� ��������� ������������� ������ ��������� ������
static void WriteTextVoltage(Chan::E ch, int x, int y);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BottomPart::Draw()
{
    int y0 = Display::HEIGHT - 19;
    int y1 = Display::HEIGHT - 10;
    int x = -1;

    Painter::DrawHLine(Grid::ChannelBottom(), 1, Grid::Left() - Measure::GetDeltaGridLeft() - 2, Color::SEPARATOR);
    Painter::DrawHLine(Grid::FullBottom(), 1, Grid::Left() - Measure::GetDeltaGridLeft() - 2);
    WriteTextVoltage(Chan::A, x + 2, y0);
    WriteTextVoltage(Chan::B, x + 2, y1);
    Painter::DrawVLine(x + 95, Grid::Bottom() + 2, Display::HEIGHT - 2, Color::SEPARATOR);

    x += 98;
    const int SIZE = 100;
    char buffer[SIZE] = {0};

    snprintf(buffer, SIZE, "�\xa5%s", TBase(SET_TBASE).ToString());

    String(buffer).Draw(x, y0, Color::FILL);

    buffer[0] = 'a';
    buffer[1] = 0;
    snprintf(buffer, SIZE, "\xa5%s", SET_TSHIFT.ToString(SET_TBASE).CString());
    String(buffer).Draw(x + 35, y0);

    buffer[0] = 0;

    if (MODE_WORK == ModeWork::Dir)
    {
        pString source[3] = { "1", "2", "\x82" };
        snprintf(buffer, 100, "�\xa5\x10%s", source[(uint8)TRIG_SOURCE]);
    }

    String(buffer).Draw(x, y1, Color::Trig());

    buffer[0] = 0;
    static pString couple[] =
    {
        "\x92",
        "\x91",
        "\x92",
        "\x92"
    };
    static pString polar[] =
    {
        "\xa7",
        "\xa6"
    };
    static pString filtr[] =
    {
        "\xb5\xb6",
        "\xb5\xb6",
        "\xb3\xb4",
        "\xb1\xb2"
    };
    if (MODE_WORK == ModeWork::Dir)
    {
        snprintf(buffer, SIZE, "\xa5\x10%s\x10\xa5\x10%s\x10\xa5\x10", couple[TRIG_INPUT], polar[TRIG_POLARITY]);
        String(buffer).Draw(x + 18, y1);

        Painter::DrawChar(x + 45, y1, filtr[TRIG_INPUT][0]);
        Painter::DrawChar(x + 53, y1, filtr[TRIG_INPUT][1]);
    }

    buffer[0] = '\0';
    const char mode[] =
    {
        '\xb7',
        '\xa0',
        '\xb0'
    };
    if (MODE_WORK == ModeWork::Dir)
    {
        snprintf(buffer, 100, "\xa5\x10%c", mode[START_MODE]);
        String(buffer).Draw(x + 63, y1);
    }

    Painter::DrawVLine(x + 79, Grid::Bottom() + 2, Display::HEIGHT - 2, Color::SEPARATOR);

    Painter::DrawHLine(Grid::Bottom(), Grid::Right() + 2, Display::WIDTH - 2, Color::FILL);
    Painter::DrawHLine(Grid::ChannelBottom(), Grid::Right() + 2, Display::WIDTH - 2);

    x += 82;
    y0 = y0 - 3;
    y1 = y1 - 6;
    int y2 = y1 + 6;
    Painter::SetFont(Font::Type::_5);

    if (MODE_WORK == ModeWork::Dir)
    {
        WriteStringAndNumber(LANG_RU ? "������" : "accum", (int16)x, (int16)y0, NUM_ACCUM);
        WriteStringAndNumber(LANG_RU ? "������" : "ave", (int16)x, (int16)y1, NUM_AVE);
        WriteStringAndNumber(LANG_RU ? "��\x93���" : "mn\x93max", (int16)x, (int16)y2, NUM_MIN_MAX);
    }

    x += 42;
    Painter::DrawVLine(x, Grid::Bottom() + 2, Display::HEIGHT - 2, Color::SEPARATOR);

    Painter::SetFont(Font::Type::_8);

    if (MODE_WORK == ModeWork::Dir)
    {
        char mesFreq[20] = "\x7c=";
        float freq = FrequencyCounter::GetFreq();
        if (freq == -1.0F) //-V550
        {
            std::strcat(mesFreq, "******");
        }
        else
        {
            std::strcat(mesFreq, Frequency(freq).ToString().CString());
        }

        String(mesFreq).Draw(x + 3, Grid::Bottom() + 2, Color::FILL);
    }

    DrawTime(x + 3, Grid::Bottom() + 11);

    Painter::DrawVLine(x + 55, Grid::Bottom() + 2, Display::HEIGHT - 2, Color::GRAY_50);

    Painter::SetFont(Font::Type::_UGO2);

    // ������
    if (FDrive::IsConnected())
    {
        Painter::Draw4SymbolsInRect(x + 57, Grid::Bottom() + 2, SYMBOL_FLASH_DRIVE);
    }

    if (CONNECTED_TO_USB || CABLE_USB_IS_CONNECTED)
    {
        Painter::Draw4SymbolsInRect(x + 72, Grid::Bottom() + 2, SYMBOL_USB, CONNECTED_TO_USB ? Color::WHITE : Color::FLASH_01);
    }

    Painter::SetColor(Color::FILL);
    // ������� ��������
    if (!SET_PEAKDET_DIS)
    {
        Painter::DrawChar(x + 38, Grid::Bottom() + 11, '\x12');
        Painter::DrawChar(x + 46, Grid::Bottom() + 11, '\x13');
    }

    if (MODE_WORK == ModeWork::Dir)
    {
        Painter::SetFont(Font::Type::_5);
        WriteStringAndNumber("�����.:", (int16)(x + 57), (int16)(Grid::Bottom() + 10), (int)ENUM_SMOOTHING + 1);
        Painter::SetFont(Font::Type::_8);
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int BottomPart::WriteChannel(Chan::E ch, int x, int y)
{
    String(Chan(ch).IsA() ? "1:" : "2:").Draw(x, y, Color::Channel(ch));

    x += 7;

    static const char symbols[3] = {SYMBOL_COUPLE_AC, SYMBOL_COUPLE_DC, SYMBOL_COUPLE_GND};

    char string[2] = {symbols[SET_COUPLE(ch)], 0};

    String(string).Draw(x, y);

    x += 8;

    String(Range(SET_RANGE(ch)).Name()).Draw(x, y);

    x += 22;

    Voltage(FPGAMath::RShift2Abs(SET_RSHIFT(ch), SET_RANGE(ch))).ToString(true).Draw(x, y);

    return x + 47;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BottomPart::WriteTBase(int x, int y)
{
    String(TBase(SET_TBASE).Name()).Draw(x, y, Color::FILL);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void WriteTextVoltage(Chan::E ch, int x, int y)
{
    if (!SET_ENABLED(ch))
    {
        return;
    }

    Color color = Color::Channel(ch);

    bool inverse = SET_INVERSE(ch);
    Divider::E divider = SET_DIVIDER(ch);
    Range::E range = SET_RANGE(ch);

    const int widthField = 91;
    const int heightField = 8;
    Color colorDraw = inverse ? Color::WHITE : color;
    if (inverse)
    {
        Painter::FillRegion(x, y, widthField, heightField, color);
    }
    const int SIZE = 100;
    char buffer[SIZE];
    snprintf(buffer, SIZE, "%s\xa5%s\xa5%s", Chan(ch).IsA() ? DICT(D1ch) : DICT(D2ch), ModeCouple(SET_COUPLE(ch)).UGO(), Range(range).ToString(divider));
    String(buffer).Draw(x + 1, y, colorDraw);
    char bufferTemp[SIZE];
    snprintf(bufferTemp, SIZE, "\xa5%s", RShift::ToString((uint16)SET_RSHIFT(ch), range, divider).CString());
    String(bufferTemp).Draw(x + 46, y);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BottomPart::WriteStringAndNumber(const char *text, int16 x, int16 y, int number)
{
    const int SIZE = 100;
    char buffer[SIZE];
    String(text).Draw(x, y, Color::FILL);
    if (number == 0)
    {
        snprintf(buffer, SIZE, "-");
    }
    else
    {
        snprintf(buffer, SIZE, "%d", number);
    }
    Painter::DrawTextRelativelyRight(x + 41, y, buffer);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BottomPart::DrawTime(int x, int y)
{
    int dField = 10;
    int dSeparator = 2;

    PackedTime time = CPU::RTC_::GetPackedTime();

    Painter::SetColor(Color::FILL);

    if ((MODE_WORK == ModeWork::ROM) || (MODE_WORK == ModeWork::RAM))
    {
        if (DS)
        {
            y -= 9;
            time.day = TIME_DAY_DS;
            time.hours = TIME_HOURS_DS;
            time.minutes = TIME_MINUTES_DS;
            time.seconds = TIME_SECONDS_DS;
            time.month = TIME_MONTH_DS;
            time.year = TIME_YEAR_DS;
            Integer((int)time.day).ToString(false, 2).Draw(x, y);
            String(':').Draw(x + dField, y);
            Integer((int)time.month).ToString(false, 2).Draw(x + dField + dSeparator, y);
            String(':').Draw(x + 2 * dField + dSeparator, y);
            Integer((int)time.year + 2000).ToString(false, 4).Draw(x + 2 * dField + 2 * dSeparator, y);

            y += 9;
        }
        else
        {
            return;
        }
    }

    Integer((int)time.hours).ToString(false, 2).Draw(x, y);
    String(':').Draw(x + dField, y);
    Integer((int)time.minutes).ToString(false, 2).Draw(x + dField + dSeparator, y);
    String(':').Draw(x + 2 * dField + dSeparator, y);
    Integer((int)time.seconds).ToString(false, 2).Draw(x + 2 * dField + 2 * dSeparator, y);
}
