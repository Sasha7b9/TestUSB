#pragma once
#include "Menu/MenuItems.h"


class PageTrig
{
public:

    static void OnChanged_TrigMode(bool active);

    static const PageBase *pointer;

    class PageHoldOff
    {
    public:
        static const PageBase *pointer;
    };
};
