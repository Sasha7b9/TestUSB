#pragma once
#include "SettingsMath.h"
#include "SettingsMemory.h"
#include "SettingsDebug.h"
#include "SettingsNRST.h"
#include "SettingsService.h"
#include "Tester/Tester.h"
#include "Osci/Cursors.h"
#include "Osci/FrequencyCounter.h"
#include "Menu/Menu.h"
#include "Display/Grid.h"
#include "Display/Colors.h"
#include "Multimeter/Multimeter.h"
#include "Utils/Measure.h"
#include "Recorder/Recorder.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SET_SIZE                    (set.size)
#define SET_CRC32                   (set.crc32)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define IN_RANDOMIZE_MODE           (SET_TBASE < TBase::_100ns)
#define CURRENT_PAGE                (set.menu_currentPage)

#define TRIG_MODE                   (set.trig_mode)
#define TRIG_HOLDOFF                (set.trig_holdOff)
#define TRIG_HOLDOFF_ENABLED        (set.trig_holdOffEnabled)

/// ���������� ������� ��������� ������ �� �������� Page::Name.
#define MENU_POS_ACT_ITEM(name)     (set.menu_posActItem[name])

#pragma pack(push, 1)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Settings
{
public:
    static void Load(bool _default = false);
    static void Save();
    static void Reset();
    /// �������� ����� �� �������� �� ���������
    static void ResetColors();
    /// ���������� ��������� �� ��������� �� ���������
    static Settings &GetDefault();
    /// ������������� ��������� � ������ ��� ���������� � ����������������� ������
    void TransformForSave(uint8 data[1024]);
    /// ������������� ����������� �� ����������������� ������ ��������� � ���������� ���
    static void TransformFromLoad(uint8 data[1024]);

    void SetData(uint8 data[1024]);
    /// ��������� �������� ������������� ����. ���������� ����� ������ ��������� ������ (���� ���� ���������� ��������� ��������)
    void SaveEnum16(uint8 name, uint8 value);

    uint                size;                               ///< ������ ������ ��������� � ������
    uint                crc32;                              ///< \brief ����������� ����� ������ ��������� � ����������� � ��� �����������

    // ���� �������
                                                                ///< ����������� �� ������� �����, ���������� �� ���� �����
    Display::ModeDrawSignal::E      disp_modeDrawSignal;
    Display::ThicknessSignal::E     disp_thickness;             ///< ������� �����, ������� �������� ������
    Display::Background::E          disp_background;            ///< ���� ����.
    Display::ShowStrNavi::E         disp_showStringNavigation;  ///< ����� ������ ������ ��������� ����.
    Display::ENumMinMax::E          disp_ENumMinMax;            ///< ������������ ���������� ��������� ��� ����������� ��������� � ���������.
    Display::ModeAveraging::E       disp_modeAveraging;         ///< ��� ���������� �� ����������.
    Display::ENumAverage::E         disp_ENumAverage;           ///< ����� ���������� �������.
    Display::ENumAccum::E           disp_ENumAccum;             ///< ����� ���������� ������� �� ������.
    Display::ModeAccumulation::E    disp_modeAccumulation;      ///< ����� ���������� ��������.
    Display::ENumSmoothing::E       disp_ENumSmoothing;         ///< ������������ ���������� ����� ��� ����������� �������.
    Display::ENumSignalsInSec::E    disp_ENumSignalsInSec;      ///< ������������ ����� ���������� ������� � �������.
    Grid::Type::E                   disp_typeGrid;              ///< ��� �����
    int16                           disp_brightnessGrid;        ///< ������� ����� �� 0 �� 100.
    Display::LinkingRShift::E       disp_linkingRShift;         ///< ��� �������� � �������� �� ���������.
    int16                           disp_brightness;            ///< ������� �������.
    int16                           disp_timeShowLevels;        ///< �����, � ������� �������� ����� ���������� ������ ��������.
    int16                           disp_timeMessages;          ///< ����� � ��������, �� ������� ��������� ������� �� �����.
    Display::AltMarkers::E          disp_altMarkers;            ///< ����� ����������� �������������� ������� �������� ��������.
    Menu::AutoHide::E               disp_menuAutoHide;          ///< ����� ������� ������� ����� ���������� ������� ������� ������� ����.
    int16                           disp_shiftInMemory;         ///< \brief ���������� �������� ������ ���� ����� ������������ �������� ����� ������ � 
                                                                ///< ������. �.�. ��� �������� ��������� ����� � ��� ���� ������ ���������� ����� �� ������.
    Chan::E                         disp_lastAffectedChannel;   ///< \brief ��������� ����������� �����. ������������ ��� ����������� ������ ��������
                                                                ///< �� ����� � ���������� ���� ������ �������

    // ���� ����� 1, ����� 2

    uint16              chan_shift[Chan::Number];           ///< ����� ������ �� ���������
    Range::E            chan_range[Chan::Number];           ///< ������� ������ �� ���������
    ModeCouple::E       chan_couple[Chan::Number];          ///< ����� �� �����
    bool                chan_enabled[Chan::Number];         ///< �������/�������� �����
    int8                chan_balanceShiftADC[2];            ///< ���������� �������� ��� ������������ ���.
    Bandwidth::E        chan_bandwidth[2];                  ///< ����������� ������.
    Resistance::E       chan_resistance[2];                 ///< ������������� �����.
    bool                chan_inverse[2];
    Divider::E          chan_divider[2];                    ///< ���������.
    CalibrationMode::E  chan_calibrationMode[2];            ///< ����� ����������.

    // ���� �������������

    Trig::Source::E     trig_source;
    Trig::Input::E      trig_input;
    Trig::Polarity::E   trig_polarity;
    uint16              trig_lev[Chan::Number];
    Trig::StartMode::E  trig_startMode;                     ///< ����� �������.
    Trig::ModeFind::E   trig_modeFind;                      ///< ����� ������������� - ������� ��� �������������.
    int                 trig_holdOff;                       ///< ��������� �������������
    bool                trig_holdOffEnabled;                ///< ��������� ��������� �������������

    // ���� ���¨����

    TShift              time_shift;
    TBase::E            time_base;
    PeakDetMode::E      time_peakDet;
    TPos::E             time_TPos;
    SampleType::E       time_sampleType;
    FunctionTime::E     time_timeDivXPos;
    LinkingTShift::E    time_linkingTShift;                 ///< ��� �������� �������� �� �����������
    SampleType::E       time_sampleTypeOld;

    // ���� �������

    bool                    curs_showCursors;                   ///< ���������� �� �������.
    Cursors::LookMode::E    curs_lookMode[2];                   ///< ������ �������� �� ��������� ��� ���� ��� ��������.
    bool                    curs_showFreq;                      ///< ������������� � true ��������, ��� ����� ���������� �� ������ 1/dT ����� ���������.
    Cursors::Active::E      curs_active;                        ///< ����� ������� ����� �������.
    Chan::E                 curs_source;                        ///< �������� - � ������ ������ ��������� �������.
    Cursors::Control::E     curs_cntrlU[Chan::Number];          ///< �������� ������� ����������.
    Cursors::Control::E     curs_cntrlT[Chan::Number];          ///< �������� ������� �������.
    Cursors::Movement::E    curs_movement;                      ///< ��� ������������ �������� - �� ������ ��� ���������.
    float                   curs_deltaU100percents[2];          ///< ���������� ����� ��������� ���������� ��� 100%, ��� ����� �������.
    float                   curs_deltaT100percents[2];          ///< ���������� ����� ��������� ������� ��� 100%, ��� ����� �������.
    float                   curs_posCurU[Chan::Number][2];      ///< ������� ������� �������� ���������� ����� �������.
    float                   curs_posCurT[Chan::Number][2];      ///< ������� ������� �������� ������� ����� �������.

    // ���� ������

#define MAX_SYMBOLS_IN_FILE_NAME 35
    ENumPointsFPGA::E   mem_enumPoints;                             ///< ����� �����
    ModeBtnMemory::E    mem_modeBtnMemory;
    ModeWork::E         mem_modeWork;                               ///< ����� ������.
    bool                mem_flashAutoConnect;                       ///< ���� true, ��� ����������� ������ ������������� ��������� ����-��������.
    int8                mem_indexCurSymbolNameMask;                 ///< ������ �������� ������� � ������ ������� ����� ��� ������ �����.
    ModeSaveSignal::E   mem_modeSaveSignal;                         ///< � ����� ���� ��������� ������.
    char                mem_fileName[MAX_SYMBOLS_IN_FILE_NAME];     ///< ��� ����� ��� ������ ������� �������.
    ModeShowIntMem::E   mem_modeShowIntMem;                         ///< ��� ���������� � ������ ����� �� - ��������� ��� ���������� ������.
    FileNamingMode::E   mem_fileNamingMode;                         ///< ����� ���������� ������.
    char                mem_fileNameMask[MAX_SYMBOLS_IN_FILE_NAME]; ///< \brief ����� ����� ��� ��������������� ���������� ������.
                        ///< \details ������� ����������.\n
                        /// \code
                        /// %y('\x42') - ���, %m('\x43') - �����, %d('\x44') - ����, %H('\x45') - ����, %M('\x46') - ������, %S('\x47') - �������
                        /// %Nn('\x48''n') - ���������� �����, ������ �������� �� ����� n ���������, ��������, 7 � %3N ����� ������������� � 007
                        /// �������
                        /// name_%4N_%y_%m_%d_%H_%M_%S ����� ������������ ����� ���� name_0043_2014_04_25_14_45_32
                        /// ��� ���� �������� ��������, ��� ���� ������������ %4N ����� ����� ��������� ����������, ��, ������ �����, ���� �������� 
                        /// ����� ������ ����� 0001, �.�. ��� ����������� ������ ���������������.
                        /// \endcode

    // ���� ���������

    bool                        meas_show;                 ///< ���������� �� ���������.
    Measure::OnDisplay::E       meas_number;               ///< ������� ��������� ��������.
    Measure::Source::E          meas_source;               ///< ��� ����� ������� �������� ���������.
    Measure::ModeViewSignals::E meas_modeViewSignals;      ///< ������� �� ������� ��� ������ ���������.
    Measure::Type::E            meas_measures[15];         ///< ��������� ��� ��������� ���������.
    Measure::Type::E            meas_marked;               ///< ���������, �� ������� ����� �������� �������.

    // ���� ������

    Language::E         serv_lang;
    Color::Scheme::E    serv_colorScheme;
    bool                serv_recorder;                  ///< ������� �� ����� ������������.
    CalibratorMode::E   serv_calibratorMode;            ///< ����� ������ �����������.
    bool                serv_soundEnable;               ///< �������� �� �����.
    int16               serv_soundVolume;               ///< ��������� ����� [0...100].

    // ��������� ����������

    FuncModeDraw::E     math_modeDraw;                  ///< ���������� ��� ����� ������� � ������ �������������� �������.
    bool                math_enableFFT;
    ScaleFFT::E         math_scaleFFT;
    SourceFFT::E        math_sourceFFT;
    WindowFFT::E        math_windowFFT;
    uint8               math_currentCursor;             ///< ����������, ����� �������� ������� ��������� ����� ���������.
    uint8               math_posCur[2];                 ///< ������� ������� �������. ���������� 0...256.
    FFTmaxDB::E         math_FFTmaxDB;
    Function::E         math_function;
    ModeRegSet::E       math_modeRegSet;                ///< ������� ����� ��������� - ������� �� ������� ��� �������� �� ���������.
    Range::E            math_range;
    Divider::E          math_divider;
    uint16              math_rShift;

    // ��������� �����������

    FrequencyCounter::Enabled::E        freq_enabled;
    FrequencyCounter::ModeView::E       freq_modeView;      
    FrequencyCounter::TimeCounting::E   freq_timeCounting;  ///< ����� ����� �������.
    FrequencyCounter::FreqClc::E        freq_freqClc;       ///< ������� ����������.
    FrequencyCounter::NumberPeriods::E  freq_numberPeriods; ///< ���������� ��������.

    // ���� �������

    int16               dbg_numStrings;                 ///< ����� ����� � �������.
    int8                dbg_sizeFont;                   ///< ������ ������ ������� - 0 - 5, 1 - 8.
    bool                dbg_modePauseConsole;           ///< ���� true, �� ����� � ������� ��������������� �������� ������ ����/����.
    bool                dbg_showAll;
    bool                dbg_showFlag;
    bool                dbg_showRShift[2];
    bool                dbg_showTrigLev;
    bool                dbg_showRange[2];
    bool                dbg_showChanParam[2];
    bool                dbg_showTrigParam;
    bool                dbg_showTShift;
    bool                dbg_showTBase;
    int16               dbg_numMeasuresForGates;        ///< ����� ��������� ��� �����.
    bool                dbg_showRandInfo;               ///< �������� ���������� �� ������������� - ������ � ��������� ��������.
    bool                dbg_showRandStat;               ///< �������� ������ ����������.
    int16               dbg_timeCompensation;           ///< �������������� �������� �� ������� ��� ������ �������� ������ �������������.
    int16               dbg_pretriggered;               ///< ������������ �������� ����������� ��� ������������ �������������.
    Bandwidth           dbg_bandwidth[2];               ///< \brief ����� ������� ������, ������� ����� ���������� ��� ������ � "�����-������" 
                                                        ///< �������� "������".
    bool                  dbg_ShowStats;                ///< ���������� ���������� �� ������ (fps, ��������).
    Display::Orientation  dbg_Orientation;

    // ��������� ������-����������

    Tester::Control::E     test_control;
    Tester::Polarity::E    test_polarity;
    Tester::StepU::E       test_stepU;
    Tester::StepI::E       test_stepI;
    Tester::Smoothing::E   test_smooth;                    ///< ���������� �����������
    Tester::ViewMode::E    test_viewMode;

    // ��������� ����

    const Page*         menu_currentPage;                               ///< ��������� �� �������� �������� ����
    bool                menu_show;                                      ///< ���� true, �� ����� ���������� ������� ��������
    int8                menu_posActItem[Page::Name::Number];            ///< \brief ������� ��������� ������. bit7 == 1 - item is opened, 0x7f - ��� 
                                                                        ///< ��������� ������.
    int8                menu_currentSubPage[Page::Name::Number];        ///< ����� ������� �����������.

    // �������������� ��������� ����������

    int16               nrst_balanceADC[Chan::Number];                  ///< �������� ��������������� �������� ��� ��� ������ ������������.
    BalanceADC          nrst_balanceADCtype;                            ///< ��� ������������.
    StretchADC          nrst_stretchADCtype;                            ///< ��� �������� ������.
    int16               nrst_StretchADC[Chan::Number][3];               ///< \brief ����������� ����������� ��� �������, �������������� �
                        ///< ������������ �������. ����� �������� � ����� ����, ����� �������� �������� �����������, ����� ��������� �� 1000 �
                        ///< ��������� �������.
    int16               nrst_rShiftAddStable[Chan::Number][3];          ///< ���������� �������� ��� ��� ����� �������������� ����������. ������� �������� ��� ���������
    int16               nrst_numAveForRand;                             ///< �� �������� ���������� ��������� ������ � ������ �������������.
    int16               nrst_numSmoothForRand;                          ///< ����� ����� ��� ����������� ������ � �������������.
    int16               nrst_rShiftAdd[Chan::Number][Range::Number][2]; ///< ���������� ��������, ������� ������� ���� ��� ���������� � ������������
    int16               nrst_correctionTime;                            ///< ����������� ��������� �������.

    // ��������� �����������
    Multimeter::AVP                 multi_avp;
    Multimeter::Measure             multi_meas;
    Multimeter::RangeDC             multi_rangeDC;
    Multimeter::RangeAC::E          multi_rangeAC;
    Multimeter::RangeResistance::E  multi_rangeResist;

    // ��������� ������������
    Recorder::ViewAlways        rec_viewAlways;         ///< ���������� �� ������ ������ ������������
    bool                        rec_sourceChanA;        ///< ��������� ������ ������ 1
    bool                        rec_sourceChanB;        ///< ��������� ������ ������ 2
    bool                        rec_sourceSensor;       ///< ��������� ������ ��������� �������
    Recorder::DataStorage       rec_storageRecord;      ///< ���� ��������� ������
    Recorder::DataStorage       rec_storagePlay;        ///< ������ ����� ���������� ������


    int16   dbg_enum_gate_max;      ///< ����������� ����� � ������������� ������
    int16   dbg_enum_gate_min;      ///< ����������� ����� � ������������� �����
};

#pragma pack(pop)

extern Settings set;
