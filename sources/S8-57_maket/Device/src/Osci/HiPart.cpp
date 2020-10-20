#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "HiPart.h"
#include "Settings/Settings.h"
#include "Display/Painter.h"
#include "Display/Symbols.h"
#include "Display/Grid.h"
#include "Utils/Dictionary.h"
#include "Utils/Values.h"
#include "FPGA/FPGA.h"
#include "FPGA/FPGAMath.h"
#include <cmath>
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HiPart::Draw()
{
    WriteCursors();
    DrawRightPart();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HiPart::WriteCursors()
{
    int startX = 43;

    if(true)
    {
        startX += 29;
    }

    int x = startX;
    int y1 = 0;
    int y2 = 9;
    if(Cursors::NecessaryDraw())
    {
        Painter::DrawVLine(x, 1, Grid::Top() - 2, Color::FILL);
        x += 3;
        Chan::E source = CURS_SOURCE;
        Color colorText = Color::Channel(source);
        if (CURsU_ENABLED)
        {
            String("1:").Draw(x, y1, colorText);
            String("2:").Draw(x, y2);
            x += 7;
            Cursors::Voltage(source, 0).Draw(x, y1);
            Cursors::Voltage(source, 1).Draw(x, y2);
            x = startX + 49;
            float pos0 = FPGAMath::VoltageCursor(Cursors::PosU(source, 0), SET_RANGE(source), SET_RSHIFT(source));
            float pos1 = FPGAMath::VoltageCursor(Cursors::PosU(source, 1), SET_RANGE(source), SET_RSHIFT(source));
            float delta = std::fabsf(pos1 - pos0);
            if(SET_DIVIDER_10(source))
            {
                delta *= 10;
            }
            String(":dU=").Draw(x, y1);
            Voltage(delta).ToString(false).Draw(x + 17, y1);
            String(':').Draw(x, y2);
            Cursors::PercentsU(source).Draw(x + 10, y2);
        }

        x = startX + 101;
        Painter::DrawVLine(x, 1, Grid::Top() - 2, Color::FILL);
        x += 3;
        if(CURsT_ENABLED)
        {
            Painter::SetColor(colorText);
            String("1:").Draw(x, y1);
            String("2:").Draw(x, y2);
            x += 7;
            Cursors::Time(source, 0).Draw(x, y1);
            Cursors::Time(source, 1).Draw(x, y2);
            x = startX + 153;          

            float pos0 = FPGAMath::TimeCursor(CURsT_POS(source, 0), SET_TBASE);
            float pos1 = FPGAMath::TimeCursor(CURsT_POS(source, 1), SET_TBASE);
            float delta = std::fabsf(pos1 - pos0);
            String(":dT=").Draw(x, y1);
            Time(delta).ToString(false).Draw(x + 17, y1);
            String(':').Draw(x, y2);
            Cursors::PercentsT(source).Draw(x + 8, y2);

            if(CURSORS_SHOW_FREQ)
            {
                int width = 65;
                x = Grid::Right() - width;
                Painter::DrawRectangle(x, Grid::Top(), width, 12, Color::FILL);
                Painter::FillRegion(x + 1, Grid::Top() + 1, width - 2, 10, Color::BACK);
                String("1/dT=").Draw(x + 1, Grid::Top() + 2, colorText);
                if (delta != 0.0F) //-V550
                {
                    Frequency(1.0F / delta).ToString().Draw(x + 25, Grid::Top() + 2);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HiPart::DrawRightPart()
{
    // �������������
    int y = 2;

    static const int xses[3] ={280, 271, 251};
    int x = xses[MODE_WORK];

    if(MODE_WORK != ModeWork::RAM)
    {
        Painter::DrawVLine(x, 1, Grid::Top() - 2, Color::FILL);

        x += 2;

        if(Trig::SyncPulse())
        {
            Painter::FillRegion(x, 1 + y, Grid::Top() - 3, Grid::Top() - 7, Color::FILL);
            String(DICT(DTrig)).Draw(x + 3, y + 3, Color::BACK);
        }
    }

    // ����� ������
    static pString strs[][2] =
    {
        {"���", "MEAS"},
        {"����", "LAST"},
        {"����", "INT"}
    };

    if(MODE_WORK != ModeWork::Dir)
    {
        x += 18;
        Painter::DrawVLine(x, 1, Grid::Top() - 2, Color::FILL);
        x += 2;
        String(DICT(DMode)).Draw(LANG_RU ? x : x + 3, -1);
        Painter::DrawStringInCenterRect(x + 1, 9, 25, 8, strs[MODE_WORK][LANG]);
    }
    else
    {
        x -= 9;
    }

    if(MODE_WORK != ModeWork::RAM)
    {
        x += 27;
        Painter::DrawVLine(x, 1, Grid::Top() - 2, Color::FILL);

        x += 2;
        y = 1;

        if(FPGA::IsRunning())       // ������� �����
        {
            Painter::Draw4SymbolsInRect(x, 1, SYMBOL_PLAY);
        }
        else if(FPGA_IN_STATE_STOP)  // ����� ���������
        {
            Painter::FillRegion(x + 3, y + 3, 10, 10);
        }
        else if(FPGA_IN_STATE_WAIT)  // ����� �������� �������
        {
            int w = 4;
            int h = 14;
            int delta = 4;
            x = x + 2;
            Painter::FillRegion(x, y + 1, w, h);
            Painter::FillRegion(x + w + delta, y + 1, w, h);
        }
        else
        {
            // ������ ������ �� ������������
        }
    }
}
