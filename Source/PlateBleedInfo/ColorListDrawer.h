//
//  ColorListDrawer.h
//  SafeguardTools
//
//  Created by T431962 on 6/23/16.
//
//

#ifndef __SafeguardTools__ColorListDrawer__
#define __SafeguardTools__ColorListDrawer__

#include "BleedTextInfoDrawer.h"
#include "ColorList.h"
#include "IDrawer.h"
#include "IDrawable.hpp"
#include "AIPluginGroup.h"

extern AIPluginGroupSuite* sAIPluginGroup;

struct ColorListDrawerSettings
{
    ColorListDrawerSettings(SafeguardFile::ProductType pt, AIRealRect artboardBounds, ColorList colorList) :
    pt(pt),
    artboardBounds(artboardBounds),
    colorList(colorList) {};
    
    SafeguardFile::ProductType pt;
    AIRealRect artboardBounds;
    ColorList colorList;
};

constexpr auto COLORLIST_ARTHANDLE =            "__bt_colorlist_arthandle__";

namespace SafeguardFile
{
    class BleedInfo;
    
    class ColorListDrawer : public BleedTextInfoDrawer
    {
    public:
        AIArtHandle Draw(AIArtHandle resultGroup) const override;
        string GetDictionaryLabel() const override { return COLORLIST_ARTHANDLE; };
    protected:
        ColorListDrawer(AIRealRect bounds, AIRealPoint anchor, ColorList colorList);
        virtual AIArtHandle DrawerSpecificSteps(AIArtHandle resultGroup) const = 0;
        
        ColorList colorList;
        AIReal maxWidth;
        AIReal maxHeight;
    };
    
    class LaserColorListDrawer : public ColorListDrawer
    {
    public:
        LaserColorListDrawer(AIRealRect bounds, ColorList colorList);
    private:
        AIArtHandle DrawerSpecificSteps(AIArtHandle resultGroup) const override;
    };
    
    class ContinuousColorListDrawer : public ColorListDrawer
    {
    public:
        ContinuousColorListDrawer(AIRealRect bounds, ColorList colorList);
    private:
        AIArtHandle DrawerSpecificSteps(AIArtHandle resultGroup) const override;
        void DrawContinuousColorBlocks(AIArtHandle resultGroup) const;
    };
    
    class BusStatColorListDrawer : public ColorListDrawer
    {
    public:
        BusStatColorListDrawer(AIRealRect bounds, ColorList colorList);
    private:
        AIArtHandle DrawerSpecificSteps(AIArtHandle resultGroup) const override;
    };
    
    class ColorListDrawable : public IDrawable
    {
    public:
        ColorListDrawable(ColorListDrawerSettings settings)
        {
            drawer = DrawerFactory().GetDrawer(settings);
        }
    };
}

using SafeguardFile::ColorListDrawable;
template <>
class DrawableFactoryImpl<ColorListDrawerSettings>
{
public:
    static std::shared_ptr<IDrawable> GetDrawable(ColorListDrawerSettings settings, AIArtHandle pluginArt)
    {
        std::shared_ptr<IDrawable> drawable = make_shared<ColorListDrawable>(settings);
        if (!pluginArt)
        {
            return drawable;
        }
        else
        {
            AIArtHandle resultGroup = NULL;
            sAIPluginGroup->GetPluginArtResultArt(pluginArt, &resultGroup);
            
            AIArtHandle existingColorListArtHandle = NULL;
            existingColorListArtHandle = DictionaryWriter(resultGroup).GetArtHandleFromIdentifier(drawable->GetDrawer()->GetDictionaryLabel());
            if (existingColorListArtHandle)
            {
                ColorList existingColorList(vector<AIColor>{});
                existingColorList.ReadColorListFromArtDictionary(existingColorListArtHandle);
                if ( existingColorList == settings.colorList )
                {
                    return nullptr;
                }
                else
                {
                    return drawable;
                }
            }
            else
            {
                return drawable;
            }
        }
    };
};


template <>
class DrawerFactoryImpl<ColorListDrawerSettings>
{
public:
    static shared_ptr<IDrawer> GetDrawer(ColorListDrawerSettings settings)
    {
        if (settings.pt == SafeguardFile::ProductType::BusinessStat)
        {
            return make_shared<SafeguardFile::BusStatColorListDrawer>(settings.artboardBounds, settings.colorList);
        }
        else if (settings.pt == SafeguardFile::ProductType::Continuous)
        {
            return make_shared<SafeguardFile::ContinuousColorListDrawer>(settings.artboardBounds, settings.colorList);
        }
        else if (settings.pt == SafeguardFile::ProductType::CutSheet || settings.pt == SafeguardFile::ProductType::Envelope)
        {
            return make_shared<SafeguardFile::LaserColorListDrawer>(settings.artboardBounds, settings.colorList);
        }
        
        return nullptr;
    };
};

#endif /* defined(__SafeguardTools__ColorListDrawer__) */
