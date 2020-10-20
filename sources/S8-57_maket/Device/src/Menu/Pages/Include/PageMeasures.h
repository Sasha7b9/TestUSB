#pragma once


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class PageMeasures
{
public:
    static const PageBase *pointer;

    static const PageBase *GetPageFrequencyCounter();

    //------------------------------------------------------------------------------------------------------------------------------------------------
    class PageAuto
    {
    public:
        static const PageBase *pointer;

        //--------------------------------------------------------------------------------------------------------------------------------------------
        class PageTune
        {
        public:
            static const PageBase *pointer;
        };
    };

    //------------------------------------------------------------------------------------------------------------------------------------------------
    class PageCursors
    {
    public:
        static const PageBase *pointer;

        //--------------------------------------------------------------------------------------------------------------------------------------------
        class PageSet
        {
        public:
            static const PageBase *pointer;

            static void Draw_Set_Channel(int x, int y);

            static void OnPress_Set_Channel();
            /// ������� ��������� ������
            static void IncCursCntrlU(Chan::E ch);
            /// ������� ��������� ������
            static void IncCursCntrlT(Chan::E ch);

            static void OnPress_Set_U();

            static void OnPress_Set_T();

            static void SetCursPos100(Chan::E ch);
            /// �������� �������� ������� ������� ���������� �� delta �����
            static void SetShiftCursPosU(Chan::E ch, int numCur, float delta);
            /// �������� �������� ������� ������� ������� �� delta �����
            static void SetShiftCursPosT(Chan::E ch, int numCur, float delta);

            static bool OnKey(KeyEvent event);

            static void UpdateCursorsForLook();
            /// ���������� ������� ������� ����������
            static void SetCursorU(Chan::E ch, int numCur, float pos);
            /// ���������� ������� ������� �������
            static void SetCursorT(Chan::E ch, int numCur, float pos);
            /// ���������� true, ���� �������� ����� ��������� ����� ���������� �������/�������
            static bool IsRegSetActiveOnCursors();

        private:
            static const float MAX_POS_U;
            static const float MAX_POS_T;

            /// ���������� �������� ��������� ���������
            static void SetCursSource(Chan::E ch);
        };
    };
};
