#include "stdafx.h"
#include "defines.h"
#include "Settings/Settings.h"
#include "Data/DataStorage.h"
#define _INCLUDE_DATA_
#include "Reader.h"
#undef _INCLUDE_DATA_
#include "FPGA/FPGA.h"
#include "Hardware/EEPROM.h"
#include "Utils/Math.h"
#include <cstring>


#ifdef WIN32
DataSettings *pDS; 
#endif


/** @addtogroup FPGA
 *  @{
 *  @addtogroup Data
 *  @{
 */


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#define __attribute__(x)
#endif
static uint8 buffer[4][16 * 1024] __attribute__((section("CCM_DATA")));

uint8 *dataIN[Chan::Number] = {buffer[0], buffer[1]};

uint8 *dataOUT[Chan::Number] = {buffer[2], buffer[3]};

void *extraMEM = 0;

static void Clear();
/// ��������� ��������� dataStruct ������� ��� ���������
static void PrepareDataForDraw(StructDataDrawing *dataStruct);
static void FillDataP2P(StructDataDrawing *dataStruct, Chan::E ch);
static void FillDataNormal(StructDataDrawing *dataStruct, Chan::E ch);
static void ReadMinMax(StructDataDrawing *dataStruct, int direction);


static DataSettings dataSettings;   ///< ����� �������� ��������� ��� �������� ��������� �������

static int numPointsP2P = 0;

/// ���� true, �� ��������� � ������ ������ ������������� � ����� �������� ����������� ������
#define STAND_P2P (IN_P2P_MODE && START_MODE_IS_WAIT && DataStorage::NumElementsInStorage() > 0)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Clear()
{
    DS = 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Reader::ReadData()
{
    dataSettings.Fill(OUT_A, OUT_B);

    pDS = &dataSettings;

    return true;

    /*
    if(DataStorage::GetData(&dataSettings))
    {
        volatile uint8 *out = 0;
        volatile uint8 *dataA = 0;
        volatile uint size = 0;

        DS = &dataSettings;
        if (DS->enableA)
        {
            out = OUT_A;
            dataA = DS->DataA();
            size = (uint)DS->SizeChannel();

            std::memcpy(OUT_A, DS->DataA(), (uint)DS->SizeChannel());

            out = out;
            dataA = dataA;
            size = size;
        }
        if (DS->enableB)
        {
            std::memcpy(OUT_B, DS->DataB(), (uint)DS->SizeChannel());
        }
        return true;
    }

    return false;
    */
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Reader::ReadFromRAM(int fromEnd, StructDataDrawing *dataStruct, bool forMemoryWindow)
{
    Clear();

    bool readed = false;      // ������� ����, ��� ������ �������

    dataStruct->posBreak = 0;

    dataStruct->forMode = ModeWork::RAM;

    if (RECORDER_MODE && FPGA::InStateStop())
    {
    }
    else if ((IN_AVERAGING_MODE || (IN_RANDOM_MODE && NRST_NUM_AVE_FOR_RAND))       // ���� �������� ����������
        && fromEnd == 0)                                                            // � ����������� ��������� ��������� ������
    {
        dataSettings = *DataStorage::DataSettingsFromEnd(0);
        DS = &dataSettings;
        if (ENABLED_DS_A)
        {
            std::memcpy(IN_A, DataStorage::GetAverageData(Chan::A), (uint)NUM_BYTES_DS);
        }
        if (ENABLED_DS_B)
        {
            std::memcpy(IN_B, DataStorage::GetAverageData(Chan::B), (uint)NUM_BYTES_DS);
        }
        readed = true;
    }
    else if(!(IN_P2P_MODE) ||                               // ���� �� � ���������� ������
            ((STAND_P2P) && !forMemoryWindow) || // -V728   // ��� � ���������� � ������ ������, �� ����� ��������� ����������� ������  
            (!FPGA::IsRunning()))                           // ��� � ����������, �� ������� ������ ����������
    {
        DataStorage::GetDataFromEnd(fromEnd, &dataSettings, IN_A, IN_B);
        readed = true;
    }
    else
    {
        // ������ ������
    }

    if (readed)
    {
        DS = &dataSettings;

        Measure::SetData(true);

        PrepareDataForDraw(dataStruct);
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Reader::ReadFromROM(StructDataDrawing *dataStruct) // -V2506
{
    Clear();

    dataStruct->posBreak = 0;

    dataStruct->forMode = ModeWork::ROM;

    if (EEPROM::GetData(NUM_ROM_SIGNAL, &dataSettings, IN_A, IN_B))
    {
        DS = &dataSettings;

        Measure::SetData(true);

        PrepareDataForDraw(dataStruct);
        
        return true;    // -V2506
    }
    
    return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Reader::ReadMin(StructDataDrawing *dataStruct)
{
    ReadMinMax(dataStruct, 0);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Reader::ReadMax(StructDataDrawing *dataStruct)
{
    ReadMinMax(dataStruct, 1);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ReadMinMax(StructDataDrawing *dataStruct, int direction)
{
    Clear();

    dataSettings = *DataStorage::DataSettingsFromEnd(0);
    
    dataStruct->needDraw[Chan::A] = dataStruct->needDraw[Chan::B] = false;

    if (DataStorage::GetLimitation(Chan::A, IN_A, direction) && DataStorage::GetLimitation(Chan::B, IN_B, direction))
    {
        DS = &dataSettings;

        Measure::SetData(false);

        PrepareDataForDraw(dataStruct);
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void PrepareDataForDraw(StructDataDrawing *dataStruct)   // -V2506
{
    if (!dataStruct)
    {
        return;
    }

    if (!DS)
    {
        dataStruct->needDraw[Chan::A] = dataStruct->needDraw[Chan::B] = false;
        return;
    }

    dataStruct->needDraw[Chan::A] = ENABLED_DS_A && SET_ENABLED_A;
    dataStruct->needDraw[Chan::B] = ENABLED_DS_B && SET_ENABLED_B;

    if (((IN_P2P_MODE) && numPointsP2P < 2 && (!STAND_P2P)) || (PEAKDET_DS != SET_PEAKDET)) // -V560
    {
        dataStruct->needDraw[Chan::A] = dataStruct->needDraw[Chan::B] = false;
        return;
    }

    if (((IN_P2P_MODE && FPGA::IsRunning() && !STAND_P2P) || (FPGA::InStateStop() && RECORDER_MODE)) && dataStruct->forMode != ModeWork::ROM)   // -V560
                                                        // FPGA_IS_RUNNING - ������ ��� � �������������� ������ ��� ���������� ������� ��������� 
    {                                                   // ���������� ��������� ����� ���������� �� ��������� �����
        FillDataP2P(dataStruct, Chan::A);
        FillDataP2P(dataStruct, Chan::B);
    }
    else
    {
        FillDataNormal(dataStruct, Chan::A);
        FillDataNormal(dataStruct, Chan::B);
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void FillDataP2P(StructDataDrawing *dataStruct, Chan::E ch)  // -V2506
{
    std::memset(dataStruct->data[ch], 0, 281 * 2);

    if (!dataStruct->needDraw[ch])
    {
        return;
    }

    int bytesInScreen = PEAKDET_DS ? 280 * 2 : 280;

    int allPoints = numPointsP2P;

    if (RECORDER_MODE)
    {
        int start = allPoints - bytesInScreen;  // ��������, ������� � �����, ����� ���������� � �������� �����.
        int end = allPoints - 1;                // ���� ��������� �������������� ����� �� �����
        LIMIT_BELOW(start, 0U);

        if (end > NUM_BYTES_DS - 1)       // ���� ������� ������ �����, ��� ���������� � ������ ������
        {
            start = NUM_BYTES_DS - bytesInScreen;
            end = NUM_BYTES_DS  - 1;
        }

        int index = start;

        for (; index <= end; index++)
        {
            dataStruct->data[ch][index - start] = OUT(ch)[index];
        }
        dataStruct->posBreak = PEAKDET_DS ? (index / 2) : (index - 1);
    }
    else
    {
        if (allPoints > 1)
        {
            int pointer = 0;                // ��������� �� ������ ������� ������
            int index = 0;                  // ��������� �� ������ ��������� ������, � ������� �� ������� 281 �����
            while (allPoints > NUM_BYTES_DS)
            {
                ++index;
                SET_IF_LARGER(index, bytesInScreen, 0);
                --allPoints;
            }
            while (allPoints > 0)
            {
                dataStruct->data[ch][index++] = OUT(ch)[pointer++];
                SET_IF_LARGER(index, bytesInScreen, 0);
                --allPoints;
            }
            dataStruct->posBreak = PEAKDET_DS ? (index / 2) : (index - 1);
        }

    }

    LIMITATION(dataStruct->posBreak, 0, 281);   // -V2516
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void FillDataNormal(StructDataDrawing *dataStruct, Chan::E ch)   // -V2506
{
    if (!dataStruct->needDraw[ch])
    {
        return;
    }

    BitSet64 points = Display::BytesOnDisplay();

    uint8 *dest = dataStruct->data[ch];
    uint8 *src = &OUT(ch)[points.word0];

    int numBytes = PEAKDET_DS ? 281 * 2 : 281;

    for(int i = 0; i < numBytes; i++)
    {
        *dest++ = *src++;
    }
}


/** @}  @}
 */
