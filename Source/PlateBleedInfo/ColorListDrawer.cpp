//
//  ColorListDrawer.cpp
//  SafeguardTools
//
//  Created by T431962 on 6/23/16.
//
//

#include "ColorListDrawer.h"
#include "ColorFuncs.h"
#include "BtAteTextFeatures.h"
#include "BtTransformArt.hpp"
#include "SafeguardFileConstants.h"
#include "BtLayer.hpp"
#include "SafeguardJobFileDTO.hpp"
#include "ArtTree.h"
#include "BleedInfo.h"

using SafeguardFile::ColorListDrawer;
using SafeguardFile::LaserColorListDrawer;
using SafeguardFile::ContinuousColorListDrawer;
using SafeguardFile::BusStatColorListDrawer;
using SafeguardFile::BleedInfo;

ColorListDrawer::ColorListDrawer(AIRealRect bounds, AIRealPoint anchor, ColorList colorList) :
    BleedTextInfoDrawer(bounds, anchor),
    colorList(colorList)
{
    maxWidth = (bounds.right - bounds.left) * .7;
    maxHeight = (bounds.top - bounds.bottom) * .7;
}

LaserColorListDrawer::LaserColorListDrawer(AIRealRect bounds, ColorList colorList) :
    ColorListDrawer(bounds, {.h = bounds.left + 4, .v = bounds.bottom - 14}, colorList) {};
ContinuousColorListDrawer::ContinuousColorListDrawer(AIRealRect bounds, ColorList colorList) :
    ColorListDrawer(bounds, {.h = bounds.right + 2, .v = bounds.top - 120}, colorList) {};
BusStatColorListDrawer::BusStatColorListDrawer(AIRealRect bounds, ColorList colorList) :
    ColorListDrawer(bounds, {.h = bounds.left, .v = bounds.bottom - 12}, colorList) {};

AIArtHandle ColorListDrawer::Draw(AIArtHandle resultGroup) const
{
    AIArtHandle colorListArt = DrawerSpecificSteps(resultGroup);
    colorList.WriteColorListToArtDictionary(colorListArt);

    return colorListArt;
}

AIArtHandle LaserColorListDrawer::DrawerSpecificSteps(AIArtHandle resultGroup) const
{
    AIArtHandle colorListArt;
    
    ATE::ITextRange range = SetupTextRange(resultGroup, kHorizontalTextOrientation, &colorListArt);
    
    colorList.AsTextRange(range, maxWidth);
    
    BtAteTextFeatures textFeatures;
    textFeatures.FontSize(12).Font("Helvetica-Bold").Justification(ATE::kLeftJustify);
    textFeatures.ApplyFeaturesToRange(range);

    return colorListArt;
}

AIArtHandle ContinuousColorListDrawer::DrawerSpecificSteps(AIArtHandle resultGroup) const
{
    AIArtHandle colorListArt;
    
    ATE::ITextRange range = SetupTextRange(resultGroup, kHorizontalTextOrientation, &colorListArt);
    
    colorList.AsTextRange(range, maxHeight);
    
    RotateArt(colorListArt, anchor, -90);
    MoveArtOutsideBounds(colorListArt, bounds, Direction::Right, 2);
    
    BtAteTextFeatures textFeatures;
    textFeatures.FontSize(9).Font("Helvetica-Bold").Justification(ATE::kLeftJustify);
    textFeatures.ApplyFeaturesToRange(range);
    
    DrawContinuousColorBlocks(resultGroup);
    
    return colorListArt;
}

AIArtHandle BusStatColorListDrawer::DrawerSpecificSteps(AIArtHandle resultGroup) const
{
    AIArtHandle colorListArt;
    
    ATE::ITextRange range = SetupTextRange(resultGroup, kHorizontalTextOrientation, &colorListArt);
    
    colorList.AsTextRange(range, maxWidth);
    
    BtAteTextFeatures textFeatures;
    textFeatures.FontSize(7).Font("Helvetica-Condensed-Bold").Justification(ATE::kLeftJustify);
    textFeatures.ApplyFeaturesToRange(range);
    
    return colorListArt;
}

void ContinuousColorListDrawer::DrawContinuousColorBlocks(AIArtHandle resultGroup) const
{
    AIRealRect rect = { .top = bounds.top - 66, .right = bounds.right - 12, .bottom = bounds.top - 66 - 12, .left = bounds.right - 12 - 12 };
    
    AIArtHandle colorBlock = DrawRectangle(rect, resultGroup);
    
    AIPathStyle currPathStyle;
    sAIPathStyle->GetPathStyle(colorBlock, &currPathStyle);
    currPathStyle.strokePaint = false;
    currPathStyle.fillPaint = true;
    currPathStyle.fill = { .color = GetRegistrationColor(), .overprint = true };
    sAIPathStyle->SetPathStyle(colorBlock, &currPathStyle);
       
    if (colorList.size() > 0 && colorList[0].Kind() == kFourColor)
    {
        vector<AIColor> cmykColors =
        {
            {.kind = kFourColor, .c.f.cyan = 1, .c.f.magenta = 0, .c.f.yellow = 0, .c.f.black = 0},
            {.kind = kFourColor, .c.f.cyan = 0, .c.f.magenta = 1, .c.f.yellow = 0, .c.f.black = 0},
            {.kind = kFourColor, .c.f.cyan = 0, .c.f.magenta = 0, .c.f.yellow = 1, .c.f.black = 0},
            {GetBlackColor()}
        };
        for (auto c : cmykColors)
        {
            rect.top -= 36;
            rect.bottom -= 36;
            
            //Make sure the blocks only go about 60% down the page
            if (! (rect.bottom < bounds.bottom * .6) )
            {
                AIArtHandle colorBlock = DrawRectangle(rect, resultGroup);
                
                AIPathStyle currPathStyle;
                sAIPathStyle->GetPathStyle(colorBlock, &currPathStyle);
                currPathStyle.strokePaint = false;
                currPathStyle.fillPaint = true;
                currPathStyle.fill = { .color = c, .overprint = true };
                sAIPathStyle->SetPathStyle(colorBlock, &currPathStyle);
            }
        }
    }
    
    if (colorList.size() > 0)
    {
        std::for_each(colorList.begin()+1, colorList.end(),
                      [&rect, resultGroup, this](BtColor c)
                      {
                          rect.top -= 36;
                          rect.bottom -= 36;
                          
                          //Make sure the blocks only go about 60% down the page
                          if (! (rect.bottom < bounds.bottom * .6) )
                          {
                              AIArtHandle colorBlock = DrawRectangle(rect, resultGroup);
                              
                              AIPathStyle currPathStyle;
                              sAIPathStyle->GetPathStyle(colorBlock, &currPathStyle);
                              currPathStyle.strokePaint = false;
                              currPathStyle.fillPaint = true;
                              currPathStyle.fill = { .color = c.AiColor(), .overprint = true };
                              sAIPathStyle->SetPathStyle(colorBlock, &currPathStyle);
                          }
                      });
    }
}

bool ShouldCreateColorListDrawable::Get() const
{
    if (!pluginArt)
    {
        return true;
    }
    else
    {
        DictionaryWriter dw(pluginArt);
        string json;
        dw.GetStringDataFromIdentifier(json, PlateBleedInfo::PLATE_BLEEDINFO);
        PlateBleedInfo::PlateDTO dto(json);
        if (settings.artboardBounds != dto.bounds)
        {
            return true;
        }
        
        AIArtHandle resultGroup = NULL;
        sAIPluginGroup->GetPluginArtResultArt(pluginArt, &resultGroup);
        
        AIArtHandle existingColorListArtHandle = NULL;
        existingColorListArtHandle = DictionaryWriter(resultGroup).GetArtHandleFromIdentifier(drawable->GetDrawer()->GetDictionaryLabel(resultGroup));
        if (existingColorListArtHandle)
        {
            ColorList existingColorList(vector<AIColor>{});
            existingColorList.ReadColorListFromArtDictionary(existingColorListArtHandle);
            if ( existingColorList == settings.colorList )
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            return true;
        }
    }
}
