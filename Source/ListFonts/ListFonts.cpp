//
//  ListFonts.cpp
//  SafeguardTools
//
//  Created by T431962 on 6/14/16.
//
//

#include "ListFonts.h"
#include "ArtTree.h"

bool ListFonts::PutFontList()
{
    AIArtSet artSet;
    sAIArtSet->NewArtSet(&artSet);
    
    MakeArtSetOfPrintingTextArts(artSet);
    
    MakeVectorOfFontsFromArtSet(artSet);
    
    RemoveDuplicatesFromFeaturesList();
    
    WriteVectorOfFontsToArtboard();
    
    sAIArtSet->DisposeArtSet(&artSet);
    
    return true;
}

long ListFonts::MakeArtSetOfPrintingTextArts(AIArtSet const targetSet)
{
    ai::ArtboardList abList;
    sAIArtboard->GetArtboardList(abList);
    ai::ArtboardID count;
    sAIArtboard->GetCount(abList, count);
    ai::ArtboardProperties props;
    
    vector<AIArtSet> artSetVector;
    
    for (int i = 0; i < count; i++)
    {
        artSetVector.push_back(AIArtSet());
        sAIArtSet->NewArtSet(&artSetVector[i]);
        
        sAIArtboard->GetArtboardProperties(abList, i, props);
        AIRealRect rect;
        props.GetPosition(rect);
        
        CreateArtSetOfPrintingObjectsWithinRect(artSetVector[i], rect);
    }
    
    for (auto& artSet : artSetVector)
    {
        size_t count;
        sAIArtSet->CountArtSet(artSet, &count);
        
        for (int i = 0; i < count; i++)
        {
            AIArtHandle currArtHandle;
            sAIArtSet->IndexArtSet(artSet, i, &currArtHandle);
            
            short type;
            sAIArt->GetArtType(currArtHandle, &type);
            
            if (type == kTextFrameArt)
            {
                sAIArtSet->AddArtToArtSet(targetSet, currArtHandle);
            }
        }
        sAIArtSet->DisposeArtSet(&artSet);
    }
    
    size_t artSetCount;
    sAIArtSet->CountArtSet(targetSet, &artSetCount);
    return artSetCount;
}


long ListFonts::MakeVectorOfFontsFromArtSet(AIArtSet const srcArtSet)
{
    size_t count;
    sAIArtSet->CountArtSet(srcArtSet, &count);
    
    for (int i = 0; i < count; i++)
    {
        AIArtHandle currArtHandle;
        sAIArtSet->IndexArtSet(srcArtSet, i, &currArtHandle);
        
        func = std::bind(&ListFonts::GetFontFromITextRange, this, std::placeholders::_1);
        ProcessTextFrameArt(currArtHandle, func);
    }
}

bool ListFonts::GetFontFromITextRange(ATE::ITextRange currRange)
{
    ATE::ITextRunsIterator iter = currRange.GetTextRunsIterator();
    while (iter.IsNotDone())
    {
        ATE::ICharFeatures features = iter.Item().GetUniqueCharFeatures();
        
        featuresList.push_back(features);
        
        iter.Next();
    }
    return true;
}

void ListFonts::WriteVectorOfFontsToArtboard()
{
    ai::ArtboardList abList;
    sAIArtboard->GetArtboardList(abList);
    ai::ArtboardProperties props;
    sAIArtboard->GetArtboardProperties(abList, 0, props);
    AIRealRect rect;
    props.GetPosition(rect);
    
    AIRealPoint anchor = {.h = rect.left, .v = rect.bottom - 52};
    AIArtHandle textFrame;
    sAITextFrame->NewPointText(kPlaceAboveAll, NULL, kHorizontalTextOrientation, anchor, &textFrame);
    
    //Create the ATE range
    ATE::TextRangeRef textRangeRef;
    sAITextFrame->GetATETextRange(textFrame, &textRangeRef);
    ATE::ITextRange textRange(textRangeRef);
    textRange.Remove();

    for (auto& feature : featuresList)
    {
        string infoString;
        bool isAssigned = false;
        
        infoString += GetFontNameFromFeatures(feature);
        
        ASReal fontSize = feature.GetFontSize(&isAssigned);
        if (isAssigned)
        {
            infoString += ", Sz: " + to_string(fontSize);
        }
        
        ASReal leading = feature.GetLeading(&isAssigned);
        if (isAssigned)
        {
            infoString += ", Ld: " + to_string(leading);
        }
        
        //Make sure the lines stay spaced apart enough to read
        ATE::ICharFeatures adjustedFeatures = feature;
        adjustedFeatures.SetLeading(fontSize + 2);
        
        infoString += "\n";
        AddTextToRangeWithFeatures(infoString, adjustedFeatures, &textRange);
    }
}

void ListFonts::RemoveDuplicatesFromFeaturesList()
{
    for(auto feature = featuresList.begin(); feature != featuresList.end(); feature++)
    {
        bool found1 = false;
        featuresList.erase(
                           std::remove_if(featuresList.begin(), featuresList.end(), [feature, &found1](ATE::ICharFeatures f)
                           {
                               bool isAAssigned = false;
                               bool isBAssigned = false;
                               
                               string fontNameA = GetFontNameFromFeatures(*feature);
                               string fontNameB = GetFontNameFromFeatures(f);
                               
                               if (fontNameA == fontNameB)
                               {
                                   ASReal fontSizeA = feature->GetFontSize(&isAAssigned);
                                   ASReal fontSizeB = f.GetFontSize(&isBAssigned);
                                   if (isAAssigned && isBAssigned && fontSizeA == fontSizeB)
                                   {
                                       ASReal leadingA = feature->GetLeading(&isAAssigned);
                                       ASReal leadingB = f.GetLeading(&isBAssigned);
                                       if (isAAssigned && isBAssigned && leadingA == leadingB)
                                       {
                                           if (!found1)
                                           {
                                               found1 = true;
                                               return false;
                                           }
                                           else
                                           {
                                               return true;
                                           }
                                       }
                                   }
                               }
                               return false;
                           }
                                          ),
                           featuresList.end()
        );
    }
}