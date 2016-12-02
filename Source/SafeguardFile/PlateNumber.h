//
//  PlateNumber.h
//  SafeguardTools
//
//  Created by T431962 on 4/7/16.
//
//

#ifndef __SafeguardTools__PlateNumber__
#define __SafeguardTools__PlateNumber__

/*
 
 enum PlantCodes {
 B,
 C,
 E,
 F,
 H,
 K,
 L,
 M,
 S,
 T,
 V,
 W,
 X,
 Y
 };
 
 */
#include "AIPath.h"
#include "AIArtboard.h"
#include "SafeguardFileConstants.h"
#include "ICanBeTextRange.h"

extern AIPathSuite* sAIPath;
extern AIArtboardSuite* sAIArtboard;

namespace SafeguardFile
{
    class PlateNumber : public ICanBeTextRange
    {
    public:
        PlateNumber(){};
        PlateNumber(string);
        
        inline Boolean IsValid() const {return isValidPlateNumber;};
        
        inline string GetPlantIndicator() const {return plantIndicator;};
        inline string GetProductIndicator() const {return productIndicator;};
        ProductType GetProductType() const;
        inline string GetSuffix() const { return suffix; };
        
        inline operator string(void) const { return plateNumber; }
        
    private:
        string plateNumber;
        string plantIndicator;
        string year;
        string productIndicator;
        string number;
        string suffix;
        Boolean isValidPlateNumber = false;
        
        Boolean TokenizePlateNumber();
        Boolean HasInnerTicks() const;
        
        void GetAsTextRange(ATE::ITextRange& targetRange, AIReal maxWidth) const override;
    };
    
}

#endif /* defined(__SafeguardTools__PlateNumber__) */
