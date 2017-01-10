//
//  BleedInfoController.hpp
//  SafeguardTools
//
//  Created by T431962 on 11/28/16.
//
//

#ifndef BleedInfoController_hpp
#define BleedInfoController_hpp

#include "AICSXSExtension.h"
#include "AIArt.h"
#include "AIPluginGroup.h"
#include "AIUndo.h"

extern AICSXSExtensionSuite* sAICSXSExtension;
extern AIArtSuite* sAIArt;
extern AIPluginGroupSuite* sAIPluginGroup;
extern AIUndoSuite* sAIUndo;

namespace PlateBleedInfo
{
    constexpr auto PLATE_BLEEDINFO_TIMESTAMP =  "__plate_bleedinfo_timestamp__";
    
    class BleedInfoController
    {
    public:
        BleedInfoController() {};
        BleedInfoController(vector<AINotifierHandle> notifiers);
        ~BleedInfoController();
        
        void HandleCropAreaNotification();
        void HandleCreateMenu();
        void HandleEditMenu();
        ASErr HandlePluginGroupNotify(AIPluginGroupMessage* message);
        ASErr HandlePluginGroupUpdate(AIPluginGroupMessage* message);

    private:
        bool SameTimestamp();
        void DrawBleedInfo();
        
        vector<AINotifierHandle> notifiers;
    };
}

#endif /* BleedInfoController_hpp */
