//
//  TickMarkDrawer.cpp
//  SafeguardTools
//
//  Created by T431962 on 6/10/16.
//
//

#include "TickMarkDrawer.h"
#include "BtLayer.hpp"

using SafeguardFile::TickMarkDrawer;

TickMarkDrawer::TickMarkDrawer(TickMarkSettings settings) : settings(settings) {};

AIArtHandle TickMarkDrawer::DoDraw() const
{
    AIArtHandle tickMarkGroupArt = NULL;
    if (settings.DrawInner())
    {
        tickMarkGroupArt = DrawInner(tickMarkGroupArt);
    }
    if (settings.DrawOuter())
    {
        tickMarkGroupArt = DrawOuter(tickMarkGroupArt);
    }
    
    return tickMarkGroupArt;
}

AIArtHandle TickMarkDrawer::DrawOuter(AIArtHandle tickMarkGroupArt) const
{
    if (tickMarkGroupArt == NULL)
    {
        BtLayer foregroundLayer(FOREGROUND_LAYER);
        AIArtHandle prep = foregroundLayer.GetLayerGroupArt();
        sAIArt->NewArt(kGroupArt, kPlaceInsideOnTop, prep, &tickMarkGroupArt);
    }
    
    vector<TickMark> tickMarks =
    {
        {
            .start = {.h = settings.Bounds().left, .v = settings.Bounds().top + settings.Offset()},
            .mid = {.h = settings.Bounds().left, .v = settings.Bounds().top},
            .end = {.h = settings.Bounds().left - settings.Offset(), .v = settings.Bounds().top}
        },
        {
            .start = {.h = settings.Bounds().right, .v = settings.Bounds().top + settings.Offset()},
            .mid = {.h = settings.Bounds().right, .v = settings.Bounds().top},
            .end = {.h = settings.Bounds().right + settings.Offset(), .v = settings.Bounds().top}
        },
        {
            .start = {.h = settings.Bounds().right, .v = settings.Bounds().bottom - settings.Offset()},
            .mid = {.h = settings.Bounds().right, .v = settings.Bounds().bottom},
            .end = {.h = settings.Bounds().right + settings.Offset(), .v = settings.Bounds().bottom}
        },
        {
            .start = {.h = settings.Bounds().left, .v = settings.Bounds().bottom - settings.Offset()},
            .mid = {.h = settings.Bounds().left, .v = settings.Bounds().bottom},
            .end = {.h = settings.Bounds().left - settings.Offset(), .v = settings.Bounds().bottom}
        }
    };

    return DrawTickMarks(tickMarks, tickMarkGroupArt);
}

AIArtHandle TickMarkDrawer::DrawInner(AIArtHandle tickMarkGroupArt) const
{
    if (tickMarkGroupArt == NULL)
    {
        BtLayer foregroundLayer(FOREGROUND_LAYER);
        AIArtHandle prep = foregroundLayer.GetLayerGroupArt();
        sAIArt->NewArt(kGroupArt, kPlaceInsideOnTop, prep, &tickMarkGroupArt);
    }
    
    vector<TickMark> tickMarks =
    {
        {
            .start = {.h = settings.Bounds().left, .v = settings.Bounds().top - settings.Offset()},
            .mid = {.h = settings.Bounds().left, .v = settings.Bounds().top},
            .end = {.h = settings.Bounds().left + settings.Offset(), .v = settings.Bounds().top}
        },
        {
            .start = {.h = settings.Bounds().right, .v = settings.Bounds().top - settings.Offset()},
            .mid = {.h = settings.Bounds().right, .v = settings.Bounds().top},
            .end = {.h = settings.Bounds().right - settings.Offset(), .v = settings.Bounds().top}
        },
        {
            .start = {.h = settings.Bounds().right, .v = settings.Bounds().bottom + settings.Offset()},
            .mid = {.h = settings.Bounds().right, .v = settings.Bounds().bottom},
            .end = {.h = settings.Bounds().right - settings.Offset(), .v = settings.Bounds().bottom}
        },
        {
            .start = {.h = settings.Bounds().left, .v = settings.Bounds().bottom + settings.Offset()},
            .mid = {.h = settings.Bounds().left, .v = settings.Bounds().bottom},
            .end = {.h = settings.Bounds().left + settings.Offset(), .v = settings.Bounds().bottom}
        }
    };
    
    return DrawTickMarks(tickMarks, tickMarkGroupArt);
}

AIArtHandle TickMarkDrawer::DrawTickMarks(vector<TickMark> ticks, AIArtHandle tickMarkGroupArt) const
{
    if (tickMarkGroupArt == NULL)
    {
        BtLayer foregroundLayer(FOREGROUND_LAYER);
        AIArtHandle prep = foregroundLayer.GetLayerGroupArt();
        sAIArt->NewArt(kGroupArt, kPlaceInsideOnTop, prep, &tickMarkGroupArt);
    }
    
    for ( auto tick : ticks )
    {
        AIArtHandle tickMarkArt;
        sAIArt->NewArt(kPathArt, kPlaceInsideOnTop, tickMarkGroupArt, &tickMarkArt);
        
        AIPathSegment segments[3];
        segments[0] = { .corner = true, .p = tick.start, .in = tick.start, .out = tick.start };
        segments[1] = { .corner = true, .p = tick.mid, .in = tick.mid, .out = tick.mid };
        segments[2] = { .corner = true, .p = tick.end, .in = tick.end, .out = tick.end };
        
        sAIPath->SetPathSegments(tickMarkArt, 0, 3, segments);
        sAIPath->SetPathClosed(tickMarkArt, false);
        
        AIPathStyle currPathStyle;
        sAIPathStyle->GetPathStyle(tickMarkArt, &currPathStyle);
        currPathStyle.stroke.color = settings.Color();
        currPathStyle.strokePaint = true;
        currPathStyle.stroke.width = settings.Weight();
        currPathStyle.fillPaint = false;
        sAIPathStyle->SetPathStyle(tickMarkArt, &currPathStyle);
    }

    return tickMarkGroupArt;
}