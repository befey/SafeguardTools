//
//  ColorList.cpp
//  SafeguardTools
//
//  Created by T431962 on 6/22/16.
//
//

#include "ColorList.h"
#include "ColorFuncs.h"
#include "BtAteTextFeatures.h"

using SafeguardFile::ColorList;

void ColorList::RemoveDuplicates()
{
    for(auto color = p_ColorList.begin(); color != p_ColorList.end(); color++)
    {
        bool found1 = false;
        p_ColorList.erase(
                        std::remove_if(p_ColorList.begin(), p_ColorList.end(), [color, &found1](AIColor c)
                                       {
                                           //Remove any Gradients and Patterns from the list
                                           if (c.kind == kPattern || c.kind == kGradient || c.kind == kNoneColor)
                                           {
                                               return true;
                                           }
                                           else if (color->kind == c.kind)
                                           {
                                               if (color->kind == kGrayColor)
                                               {
                                                   if (color->c.g == c.c.g)
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
                                               else if (color->kind == kFourColor)
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
                                               else if (color->kind == kCustomColor)
                                               {
                                                   if (color->c.c.color == c.c.c.color)
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
                                               else if (color->kind == kThreeColor)
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
                                           else if (color->kind == kFourColor && ColorIsBlack(c))
                                           {
                                               return true;
                                           }
                                           return false;
                                       }
                                       ),
                        p_ColorList.end()
                        );
    }
}

void ColorList::RemoveNonPrintingColors()
{
    p_ColorList.erase(
                    std::remove_if(p_ColorList.begin(), p_ColorList.end(), [this](AIColor c)
                                   {
                                       if (ColorIsNonPrinting(c))
                                       {
                                           return true;
                                       }
                                       else if ((GetColorName(c) == REGISTRATION_COLOR_NAME) && (p_ColorList.size() > 1))
                                       {
                                           return true;
                                       }
                                       return false;
                                   }
                                   ),
                    p_ColorList.end()
                    );
}

void ColorList::AddColorsToList(vector<AIColor> colors)
{
    p_ColorList.insert(std::end(p_ColorList), std::begin(colors), std::end(colors));
}

void ColorList::GetAsTextRange(ATE::ITextRange& targetRange) const
{
    if (p_ColorList.size() == 0)
    {
        AIColor c = {.kind = kNoneColor};
        AddColorToTextRange(c, targetRange);
    }
    
    for (auto c : p_ColorList)
    {
        AddColorToTextRange(c, targetRange);
    }
}

void ColorList::AddColorToTextRange(const AIColor color, ATE::ITextRange& targetRange) const
{
    BtAteTextFeatures textFeatures;
    
    if (color.kind == kNoneColor)
    {
        textFeatures.SetFillColor(GetRegistrationColor());
        textFeatures.AddTextToRangeWithFeatures("NO IMPRINT", targetRange);
    }
    else
    {
        string name;
        if (ColorIsPantone(color))
        {
            name = GetInnerPantoneColorNumber(color);
            textFeatures.SetFillColor(color);
        }
        else if (color.kind == kFourColor)
        {
            AIColor c = {.kind = kFourColor, .c.f.cyan = 1, .c.f.magenta = 0, .c.f.yellow = 0, .c.f.black = 0};
            textFeatures.SetFillColor(c);
            textFeatures.AddTextToRangeWithFeatures("CYAN  ", targetRange);
            c = {.kind = kFourColor, .c.f.cyan = 0, .c.f.magenta = 1, .c.f.yellow = 0, .c.f.black = 0};
            textFeatures.SetFillColor(c);
            textFeatures.AddTextToRangeWithFeatures("MAG  ", targetRange);
            c = {.kind = kFourColor, .c.f.cyan = 0, .c.f.magenta = 0, .c.f.yellow = 1, .c.f.black = 0};
            textFeatures.SetFillColor(c);
            textFeatures.AddTextToRangeWithFeatures("YEL  ", targetRange);
            textFeatures.SetFillColor(GetBlackColor());
            name = GetColorName(GetBlackColor());
        }
        else
        {
            name = GetColorName(color);
            textFeatures.SetFillColor(color);
        }
        
        textFeatures.AddTextToRangeWithFeatures((ai::UnicodeString(name).toUpper()).as_Platform() + "  ", targetRange);
    }
}