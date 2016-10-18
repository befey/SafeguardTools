//
//  BleedInfoDrawer.cpp
//  SafeguardTools
//
//  Created by T431962 on 6/23/16.
//
//

#include "BleedInfoDrawer.h"
#include "SafeguardToolsPlugin.h"
#include "ArtTree.h"
#include "BtLayer.hpp"
#include "GetIllustratorErrorCode.h"


using SafeguardFile::BleedInfoDrawer;

AIArtHandle BleedInfoDrawer::DoDraw(AIArtHandle pluginGroupArt) const
{
    if (pluginGroupArt)
    {
        return Update(pluginGroupArt);
    }
    else
    {
        return Add();
    }
    return NULL;
}

AIArtHandle BleedInfoDrawer::Add() const
{
    AIArtHandle pluginGroupArt;
    
    BtLayer foregroundLayer(FOREGROUND_LAYER);
    AIArtHandle prep = foregroundLayer.GetLayerGroupArt();
    
    AIErr err = sAIArt->NewArt(kPluginArt, kPlaceInsideOnBottom, prep, &pluginGroupArt);
    string error = GetIllustratorErrorCode(err);
    sAIPluginGroup->UseAIPluginGroup(pluginGroupArt, gPlugin->GetBleedInfoPluginGroupHandle());
    
    CreateResultArt(pluginGroupArt);
    
    return pluginGroupArt;
}

AIArtHandle BleedInfoDrawer::Update(AIArtHandle pluginGroupArt) const
{
    CreateResultArt(pluginGroupArt);
    
    return pluginGroupArt;
}

AIArtHandle BleedInfoDrawer::CreateResultArt(AIArtHandle pluginGroupArt) const
{
    AIArtHandle resultGroup;
    sAIPluginGroup->GetPluginArtResultArt(pluginGroupArt, &resultGroup);
    
    ClearResultArt(resultGroup);
    
    for (auto drawer : drawers)
    {
        drawer->Draw(resultGroup);
    }
    
    return pluginGroupArt;
}

void BleedInfoDrawer::ClearResultArt(AIArtHandle resultGroupArt) const
{
    // Destroy all art that exists below the result art.
    AIArtHandle child = NULL;
    sAIArt->GetArtFirstChild(resultGroupArt, &child);
    while (child != NULL)
    {
        sAIArt->DisposeArt(child);
        sAIArt->GetArtFirstChild(resultGroupArt, &child);
    }
}

void BleedInfoDrawer::Remove(AIArtHandle& pluginGroupArt) const
{
    sAIArt->DisposeArt(pluginGroupArt);
    pluginGroupArt = NULL;
}
