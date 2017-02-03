//
//  PrintToPdfFolderPrefsUIController.hpp
//  SafeguardTools
//
//  Created by T431962 on 2/1/17.
//
//

#ifndef PrintToPdfFolderPrefsUIController_hpp
#define PrintToPdfFolderPrefsUIController_hpp

//=================================
// included dependencies
#include <string>
#include "SDKPlugPlug.h"
#include "FlashUIController.h"
#include "AIDocument.h"

//=================================
// forward declared dependencies
class SafeguardToolsPlugin;
extern SafeguardToolsPlugin *gPlugin;

extern AIDocumentSuite* sAIDocument;

namespace PrintToPdf
{
    //=================================
    // PrintToPdfFolderPrefsUIController - handles communication between extension and plugin
    class PrintToPdfFolderPrefsUIController : public FlashUIController
    {
    public:
        PrintToPdfFolderPrefsUIController(void);
        
        /** Registers the events this plug-in will listen for with PlugPlug.
         @return error code if error found, or success otherwise.
         */
        csxs::event::EventErrorCode RegisterCSXSEventListeners();
        
        /** Removes the previously added event listeners from PlugPlug.
         @return error code if error found, or success otherwise.
         */
        csxs::event::EventErrorCode RemoveEventListeners();
        
        ASErr SendData();
        
        void ParseData(const char* eventData);
        
        static void PanelLoaded (const csxs::event::Event* const event, void* const context);
        static void OkButtonClickedFunc (const csxs::event::Event* const event, void* const context);
        static void CancelButtonClickedFunc (const csxs::event::Event* const event, void* const context);
        
        ASErr SendFolderPrefsToHtml();
        string GetPrintToPdfFolderPrefsAsXml();
        void SendCloseMessageToHtml();
        
        //=================================
        // Constant definitions
        static constexpr auto PRINTTOPDF_FOLDERPREFS_EXTENSION =  "com.gosafeguard.SafeguardTools.PrintToPdf.FolderPrefs";
        static constexpr auto EVENT_TYPE_OK_CLICKED =             "com.gosafeguard.SafeguardTools.PrintToPdf.FolderPrefs.okbutton";
        static constexpr auto EVENT_TYPE_CANCEL_CLICKED =         "com.gosafeguard.SafeguardTools.PrintToPdf.FolderPrefs.cancelbutton";
        static constexpr auto EVENT_TYPE_RESULTS_BACK =           "com.gosafeguard.SafeguardTools.PrintToPdf.FolderPrefs.datafromplugin";
        static constexpr auto EVENT_TYPE_FORCE_PANEL_CLOSE =      "com.gosafeguard.SafeguardTools.PrintToPdf.FolderPrefs.forcepanelclose";
        static constexpr auto EVENT_TYPE_PANEL_LOADED =           "com.gosafeguard.SafeguardTools.PrintToPdf.FolderPrefs.panelloaded";
        static constexpr auto ILST_APP =                          "ILST";
        
    private:
        std::pair<string, ai::FilePath> MakePresetPathPair(string preset, string defaultPath);
    };
}


#endif /* PrintToPdfFolderPrefsUIController_hpp */
