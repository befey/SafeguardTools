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
#include "SafeguardFileConstants.h"

using SafeguardFile::ColorList;

void ColorList::RemoveDuplicates()
{
    if (p_ColorList.size() <= 1)
    {
        return;
    }
    for(auto color = p_ColorList.begin(); color != p_ColorList.end(); color++)
    {
        bool found1 = false;
        p_ColorList.erase(
                          std::remove_if(color, p_ColorList.end(), [color, &found1](BtColor c)
                                         {
                                             //Remove any Gradients and Patterns from the list
                                             if (c.Kind() == kPattern || c.Kind() == kGradient || c.Kind() == kNoneColor)
                                             {
                                                 return true;
                                             }
                                             else if (color->Kind() == c.Kind() && color->Method() == c.Method())
                                             {
                                                 if (color->Kind() == kGrayColor)
                                                 {
                                                     if (color->AiColor().c.g == c.AiColor().c.g)
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
                                                 else if (color->Kind() == kFourColor)
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
                                                 else if (color->Kind() == kCustomColor)
                                                 {
                                                     if (color->AiColor().c.c.color == c.AiColor().c.c.color)
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
                                                 else if (color->Kind() == kThreeColor)
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
                                             else if (color->Kind() == kFourColor && ColorIsBlack(c.AiColor()))
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
                    std::remove_if(p_ColorList.begin(), p_ColorList.end(), [this](BtColor c)
                                   {
                                       if (ColorIsNonPrinting(c.AiColor()))
                                       {
                                           return true;
                                       }
                                       else if ((c.Name() == REGISTRATION_COLOR_NAME) && (p_ColorList.size() > 1))
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
    std::sort(p_ColorList.begin(), p_ColorList.end());
}

void ColorList::AddColorsToList(ColorList colors)
{
    p_ColorList.insert(p_ColorList.end(), colors.p_ColorList.begin(), colors.p_ColorList.end());
    std::sort(p_ColorList.begin(), p_ColorList.end());
}

void ColorList::GetAsTextRange(ATE::ITextRange& targetRange, AIReal maxWidth) const
{
    if (p_ColorList.size() == 0)
    {
        AIColor c = {.kind = kNoneColor};
        AddColorToTextRange(c, targetRange, maxWidth);
    }
    
    for (auto c : GetColorList())
    {
        AddColorToTextRange(c, targetRange, maxWidth);
    }
}

void ColorList::AddColorToTextRange(const BtColor color, ATE::ITextRange& targetRange, AIReal maxWidth) const
{
    BtAteTextFeatures textFeatures;
    
    if (color.Kind() == kNoneColor)
    {
        textFeatures.FillColor(GetRegistrationColor());
        textFeatures.AddTextToRangeWithFeatures("NO IMPRINT", targetRange);
    }
    else
    {
        string name;
        if (ColorIsPantone(color.AiColor()))
        {
            name = GetInnerPantoneColorNumber(color.AiColor());
            textFeatures.FillColor(color.AiColor());
        }
        else if (color.Kind() == kFourColor)
        {
            AIColor c = {.kind = kFourColor, .c.f.cyan = 1, .c.f.magenta = 0, .c.f.yellow = 0, .c.f.black = 0};
            textFeatures.FillColor(c);
            textFeatures.AddTextToRangeWithFeatures("CYAN  ", targetRange);
            c = {.kind = kFourColor, .c.f.cyan = 0, .c.f.magenta = 1, .c.f.yellow = 0, .c.f.black = 0};
            textFeatures.FillColor(c);
            textFeatures.AddTextToRangeWithFeatures("MAG  ", targetRange);
            c = {.kind = kFourColor, .c.f.cyan = 0, .c.f.magenta = 0, .c.f.yellow = 1, .c.f.black = 0};
            textFeatures.FillColor(c);
            textFeatures.AddTextToRangeWithFeatures("YEL  ", targetRange);
            textFeatures.FillColor(GetBlackColor());
            name = GetColorName(GetBlackColor());
        }
        else
        {
            name = color.Name();
            textFeatures.FillColor(color.AiColor());
        }
        if (color.Method() != SafeguardFile::InkMethod::NONE)
        {
            name += " " + InkMethodStrings.at(color.Method());
        }
        
        ATETextDOM::Int32 beforeEnd = targetRange.GetEnd();
        textFeatures.NoBreak(true);
        textFeatures.AddTextToRangeWithFeatures((ai::UnicodeString(name).toUpper()).as_Platform(), targetRange);
        textFeatures.NoBreak(false);
        textFeatures.AddTextToRangeWithFeatures("  ", targetRange);
        ATETextDOM::Int32 afterEnd = targetRange.GetEnd();
        
        AIRealRect bounds;
        sAIATETextUtil->GetBoundsFromTextRange(targetRange.GetRef(), &bounds);
        if ( (bounds.right - bounds.left) > maxWidth )
        {
            targetRange.SetEnd(beforeEnd);
            targetRange.InsertAfter(ai::UnicodeString("\n").as_ASUnicode().c_str());
            ATETextDOM::Int32 newEnd = targetRange.GetEnd();
            targetRange.SetEnd(newEnd - beforeEnd + afterEnd);
        }
    }
}

void ColorList::SetColorMethod(string colorName, SafeguardFile::InkMethod method)
{
    for (auto& color : p_ColorList )
    {
        if (color.CompareName(colorName))
        {
            color.Method(method);
            break;
        }
    }
}
