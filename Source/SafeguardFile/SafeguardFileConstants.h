//
//  SafeguardFileConstants.h
//  SafeguardTools
//
//  Created by T431962 on 7/15/16.
//
//

#ifndef SafeguardFileConstants_h
#define SafeguardFileConstants_h

#include "BtColor.h"
#include <unordered_map>

namespace SafeguardFile
{
    constexpr auto REGISTRATION_COLOR_NAME =    "[Registration]";
    constexpr auto NONE_COLOR_NAME =            "[None]";
    constexpr auto BLACK_COLOR_NAME =           "Black";
    constexpr auto WHITE_COLOR_NAME =           "White";
    constexpr auto MICR_BLACK_MAG_COLOR_NAME =  "MICR - BLACK MAG";
    constexpr auto GRIPPER_COLOR_NAME =         "GRIPPER";
    
    const std::unordered_map<std::string, BtColor> StdColorDefinitions
    {
        { BLACK_COLOR_NAME,
          { BLACK_COLOR_NAME,
            kCustomFourColor,
            {.f.cyan = 0, .f.yellow = 0, .f.magenta = 0, .f.black = 1},
            0
          }
        },
        { WHITE_COLOR_NAME,
          { WHITE_COLOR_NAME,
            kCustomFourColor,
            {.f.cyan = 0, .f.yellow = 0, .f.magenta = 0, .f.black = 0},
            0
          }
        },
        { MICR_BLACK_MAG_COLOR_NAME,
          { MICR_BLACK_MAG_COLOR_NAME,
            kCustomFourColor,
            {.f.cyan = 0, .f.yellow = 0, .f.magenta = 0, .f.black = 1},
            0
          }
        },
     };
    
    enum ProductType {
        INVAL,
        CutSheet,
        Continuous,
        Snapset,
        BusinessStat
    };
    
    static constexpr auto LENGTH_OF_INNER_TICK_PATH = 18;

    constexpr auto KEYLINE_LAYER =              "KEY LINE";
    constexpr auto REG_LAYER =                  "Registration Guide for Proof";
    constexpr auto SLUG_LAYER =                 "SLUG";
    constexpr auto FOREGROUND_LAYER =           "Foreground";
    constexpr auto GUIDE_LAYER =                "Guides";
    constexpr auto BACKGROUND_LAYER =           "Background";
    
    static constexpr auto NO_TOKEN_DESIG =      "F";
}


#endif /* SafeguardFileConstants_h */