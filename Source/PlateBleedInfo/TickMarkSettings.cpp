//
//  TickMarkSettings.cpp
//  SafeguardTools
//
//  Created by T431962 on 7/26/16.
//
//

#include "TickMarkSettings.hpp"
#include "ColorFuncs.h"
#include "BtColor.h"

using SafeguardFile::TickMarkSettings;

TickMarkSettings::TickMarkSettings()
: TickMarkSettings(AIRealRect(), SafeguardFile::ProductType::INVAL, SafeguardFile::TickMarkStyle::NONE) {}

TickMarkSettings::TickMarkSettings(const AIRealRect rect, const ProductType pt, const SafeguardFile::TickMarkStyle tms)
: bounds(rect), color(BtColor::GetRegistrationColor())
{
    if (pt == ProductType::Continuous)
    {
        tmStyle = TickMarkStyle::Both;
    }
    else if (pt == ProductType::Snapset)
    {
        tmStyle = TickMarkStyle::Inner;
    }
    else if (pt == ProductType::CutSheet)
    {
        tmStyle = TickMarkStyle::Outer;
    }
    else
    {
        tmStyle = TickMarkStyle::NONE;
    }
    
    if (pt == ProductType::Snapset || pt == ProductType::Continuous)
    {
        offset = TICK_LENGTH_CONTINUOUS;
    }
    else
    {
        offset = TICK_LENGTH_CUTSHEET;
        color.c.c.tint = TICK_TINT_CUTSHEET;
    }
}

bool TickMarkSettings::DrawInner() const
{
    return tmStyle == TickMarkStyle::Both || tmStyle == TickMarkStyle::Inner;
}

bool TickMarkSettings::DrawOuter() const
{
    return tmStyle == TickMarkStyle::Both || tmStyle == TickMarkStyle::Outer;
}
