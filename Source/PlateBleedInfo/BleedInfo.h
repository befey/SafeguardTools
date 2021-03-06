//
//  BleedInfo.h
//  SafeguardTools
//
//  Created by T431962 on 6/7/16.
//
//

#ifndef __SafeguardTools__BleedInfo__
#define __SafeguardTools__BleedInfo__

#include "AIUser.h"
#include "ColorList.h"
#include "PlateNumber.h"
#include "TickMarkSettings.hpp"
#include <ctime>
#include "AIDocument.h"
#include "AIArtboard.h"
#include "IDrawable.hpp"

extern AIDocumentSuite* sAIDocument;
extern AIArtboardSuite* sAIArtboard;

namespace PlateBleedInfo
{
    class SafeguardJobFileDTO;
    class PlateDTO;
    
    constexpr auto PLATE_BLEEDINFO =            "__plate_bleedinfo__";
    constexpr auto SG_BLEEDINFO_ARTHANDLES =    "__sg_bleedinfo_arthandles__";
    
    class BleedInfo : public IDrawable
    {
    public:
        BleedInfo(ai::ArtboardID artboardIndex, bool redrawAllWithoutCheck) : BleedInfo(artboardIndex, redrawAllWithoutCheck, nullptr) {};
        BleedInfo(ai::ArtboardID artboardIndex, bool redrawAllWithoutCheck, const PlateBleedInfo::PlateDTO* dto);
        ~BleedInfo();
        
        ai::ArtboardID ArtboardIndex() const { return artboardIndex; };
        AIRealRect ArtboardBounds() const;
        AIArtHandle BleedInfoPluginArtHandle() const { return bleedInfoPluginArt; };
        BleedInfo& BleedInfoPluginArtHandle(AIArtHandle newVal) { bleedInfoPluginArt = newVal; return *this; };
        tm LastModified() const;
        
        const SafeguardFile::TickMarkSettings TickMarkSettings() const { return tmSettings; };
        BleedInfo& TickMarkStyle(SafeguardFile::TickMarkStyle newVal) { tmSettings.TickMarkStyle(newVal); return *this; };
        
        string Token() const;
        BleedInfo& Token(string newVal);
        string ArtboardName(bool& isDefault) const;
        string ArtboardName() const;
        BleedInfo& ArtboardName(string newVal);
        
        bool ShouldDrawBleedInfo() const { return shouldDrawBleedInfo; };
        BleedInfo& ShouldDrawBleedInfo(bool newVal) { shouldDrawBleedInfo = newVal; return *this; };
        bool ShouldAddCmykBlocks() const { return shouldAddCMYKBlocks; };
        BleedInfo& ShouldAddCmykBlocks(bool newVal) { shouldAddCMYKBlocks = newVal; return *this; };
        bool ShouldPrint() const { return shouldPrint; };
        bool RedrawAllWithoutCheck() const { return redrawAllWithoutCheck; };
        BleedInfo& ShouldPrint(bool newVal) { shouldPrint = newVal; return *this; };
        const SafeguardFile::PlateNumber PlateNumber() const { return plateNumber; };
        class ColorList& ColorList() { return colorList; };
        const class ColorList& ColorList() const { return colorList; };
    
        AIRealRect Bleeds() const;
        
        void StoreInPluginArt() const;
        void ReadFromPluginArt();
        
        AIArtHandle Draw(AIArtHandle existingArt = nullptr) override;
        void Remove();
    private:
        bool shouldDrawBleedInfo = true;
        bool shouldAddCMYKBlocks = false;
        bool shouldPrint = true;
        
        bool redrawAllWithoutCheck = false;
        
        AIArtHandle bleedInfoPluginArt = nullptr;
        
        ai::ArtboardID artboardIndex;
        class ColorList colorList;
        SafeguardFile::PlateNumber plateNumber;
        SafeguardFile::TickMarkSettings tmSettings;
        
        void SetPlateNumber();
        void SetPlateNumber(string pn);
        void SetShouldPrint();
        
        void FillBleedInfoFromPlateDTO(const PlateBleedInfo::PlateDTO* dto, bool changeArtboardName);
    };
}

#endif /* defined(__SafeguardTools__BleedInfo__) */
