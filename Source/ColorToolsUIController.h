//
//  ColorToolsUIHandler.h
//  ColorTools
//
//  Created by T431962 on 5/19/15.
//
//

#ifndef __ColorTools__ColorToolsUIController__
#define __ColorTools__ColorToolsUIController__

#include "SDKPlugPlug.h"
#include "FlashUIController.h"


#define COLORTOOLS_UI_EXTENSION             "com.gosafeguard.ColorToolsUI"
#define EVENT_TYPE_CHANGE_CLICKED           "com.gosafeguard.ColorToolsUI.changebutton"
#define EVENT_TYPE_REMOVE_CLICKED           "com.gosafeguard.ColorToolsUI.removebutton"
#define EVENT_TYPE_UPDATE_COLOR_LIST        "com.gosafeguard.ColorToolsUI.updatelist"
#define EVENT_TYPE_UPDATE_COLOR_LIST_BACK   "com.gosafeguard.ColorToolsUI.updatelistback"
#define EVENT_TYPE_CHANGE_COUNT_BACK        "com.gosafeguard.ColorToolsUI.changecountback"
#define ILST_APP                            "ILST"


class ColorToolsPlugin;
extern ColorToolsPlugin *gPlugin;

static void ChangeButtonClickedFunc (const csxs::event::Event* const event, void* const context);
static void RemoveButtonClickedFunc (const csxs::event::Event* const event, void* const context);

class ColorToolsUIController : public FlashUIController
{
public:
    ColorToolsUIController(void);
    
    /** Registers the events this plug-in will listen for with PlugPlug.
     @return error code if error found, or success otherwise.
     */
    csxs::event::EventErrorCode RegisterCSXSEventListeners();
    
    /** Removes the previously added event listeners from PlugPlug.
     @return error code if error found, or success otherwise.
     */
    csxs::event::EventErrorCode RemoveEventListeners();
    
    ASErr SendData();
    ASErr SendColorListXmlToHtml(string swatchesXml);
    ASErr SendChangeCountToHtml(int count);
        
    void ParseData(const char* eventData);
    
    static void ChangeButtonClickedFunc (const csxs::event::Event* const event, void* const context);
    static void RemoveButtonClickedFunc (const csxs::event::Event* const event, void* const context);
    static void UpdateListFunc (const csxs::event::Event* const event, void* const context);
};

#endif /* defined(__ColorTools__ColorToolsUIController__) */
