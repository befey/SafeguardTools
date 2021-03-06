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
#include "BleedInfo.h"

extern AIPluginGroupSuite* sAIPluginGroup;

struct ColorListDrawerSettings
{
    ColorListDrawerSettings(PlateBleedInfo::BleedInfo bleedInfo)
    :
    pt(bleedInfo.PlateNumber().GetProductType()),
    artboardBounds(bleedInfo.ArtboardBounds()),
    colorList(bleedInfo.ColorList()),
    shouldDrawBleedInfo(bleedInfo.ShouldDrawBleedInfo()),
    redrawAllWithoutCheck(bleedInfo.RedrawAllWithoutCheck()) {};
    
    SafeguardFile::ProductType pt;
    AIRealRect artboardBounds;
    ColorList colorList;
    bool shouldDrawBleedInfo;
    bool redrawAllWithoutCheck;
};

constexpr auto COLORLIST_ARTHANDLE =            "__bt_colorlist_arthandle__";

namespace SafeguardFile
{
    class BleedInfo;
    
    class ColorListDrawer : public BleedTextInfoDrawer
    {
    public:
        AIArtHandle Draw(AIArtHandle resultGroup) const override;
        string GetDictionaryLabel(AIArtHandle resultArt) const override { return COLORLIST_ARTHANDLE + DictionaryWriter::GetUIDStringForArt(resultArt); };
    protected:
        ColorListDrawer(AIRealRect artboardBounds, AIRealPoint anchor, ColorList colorList);
        virtual AIArtHandle DrawerSpecificSteps(AIArtHandle resultGroup) const = 0;
        
        ColorList colorList;
    };
    
    class LaserColorListDrawer : public ColorListDrawer
    {
    public:
        LaserColorListDrawer(AIRealRect artboardBounds, ColorList colorList);
    private:
        AIArtHandle DrawerSpecificSteps(AIArtHandle resultGroup) const override;
    };
    
    class ContinuousColorListDrawer : public ColorListDrawer
    {
    public:
        ContinuousColorListDrawer(AIRealRect artboardBounds, ColorList colorList);
    private:
        AIArtHandle DrawerSpecificSteps(AIArtHandle resultGroup) const override;
        void DrawContinuousColorBlocks(AIArtHandle resultGroup) const;
    };
    
    class BusStatColorListDrawer : public ColorListDrawer
    {
    public:
        BusStatColorListDrawer(AIRealRect artboardBounds, ColorList colorList);
    private:
        AIArtHandle DrawerSpecificSteps(AIArtHandle resultGroup) const override;
    };
    
    class NoneColorListDrawer : public ColorListDrawer
    {
    public:
        NoneColorListDrawer() : ColorListDrawer({}, {}, ColorList(vector<AIColor>{})) {};
        AIArtHandle Draw(AIArtHandle resultGroup) const override { return nullptr; };
    private:
        AIArtHandle DrawerSpecificSteps(AIArtHandle resultGroup) const override { return nullptr; };
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

class ShouldCreateColorListDrawable
{
public:
    ShouldCreateColorListDrawable(std::shared_ptr<IDrawable>drawable, ColorListDrawerSettings settings, AIArtHandle pluginArt) : drawable(drawable), settings(settings), pluginArt(pluginArt) {};
    bool Get() const;
private:
    std::shared_ptr<IDrawable> drawable;
    ColorListDrawerSettings settings;
    AIArtHandle pluginArt;
};

using SafeguardFile::ColorListDrawable;
template <>
class DrawableFactoryImpl<ColorListDrawerSettings>
{
public:
    static std::shared_ptr<IDrawable> GetDrawable(ColorListDrawerSettings settings, AIArtHandle pluginArt)
    {
        std::shared_ptr<IDrawable> drawable = make_shared<ColorListDrawable>(settings);
        
        if ( ShouldCreateColorListDrawable(drawable, settings, pluginArt).Get() )
        {
            return drawable;
        }
        
        return nullptr;
    };
};


template <>
class DrawerFactoryImpl<ColorListDrawerSettings>
{
public:
    static shared_ptr<IDrawer> GetDrawer(ColorListDrawerSettings settings)
    {
        if (!settings.shouldDrawBleedInfo)
        {
            return make_shared<SafeguardFile::NoneColorListDrawer>();
        }
        
        if (settings.pt == SafeguardFile::ProductType::BusinessStat)
        {
            return make_shared<SafeguardFile::BusStatColorListDrawer>(settings.artboardBounds, settings.colorList);
        }
        else if (settings.pt == SafeguardFile::ProductType::Continuous)
        {
            return make_shared<SafeguardFile::ContinuousColorListDrawer>(settings.artboardBounds, settings.colorList);
        }
        else if (settings.pt == SafeguardFile::ProductType::CutSheet || settings.pt == SafeguardFile::ProductType::Envelope || settings.pt == SafeguardFile::ProductType::Snapset)
        {
            return make_shared<SafeguardFile::LaserColorListDrawer>(settings.artboardBounds, settings.colorList);
        }
        
        return make_shared<SafeguardFile::NoneColorListDrawer>();
    };
};

#endif /* defined(__SafeguardTools__ColorListDrawer__) */
