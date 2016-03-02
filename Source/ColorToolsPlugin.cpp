#include "IllustratorSDK.h"
#include "ColorToolsPlugin.h"
#include "ColorToolsSuites.h"

#include "AICSXS.h"

#include "ColorToolsUIController.h"
#include "BtSwatchList.h"
#include "FixBlack.h"

ColorToolsPlugin *gPlugin = NULL;

Plugin* AllocatePlugin(SPPluginRef pluginRef)
{
	return new ColorToolsPlugin(pluginRef);
}

void FixupReload(Plugin* plugin)
{
	ColorToolsPlugin::FixupVTable((ColorToolsPlugin*) plugin);
}

ColorToolsPlugin::ColorToolsPlugin(SPPluginRef pluginRef) :
	Plugin(pluginRef),
    fRegisterEventNotifierHandle(NULL),
    colorToolsUIController(NULL),
    mySwatchList(NULL)
{
	strncpy(fPluginName, kColorToolsPluginName, kMaxStringLength);
}

ColorToolsPlugin::~ColorToolsPlugin()
{
}

ASErr ColorToolsPlugin::Message(char* caller, char* selector, void *message)
{
	ASErr error = kNoErr;
    
	try {
		error = Plugin::Message(caller, selector, message);
	}
	catch (ai::Error& ex) {
		error = ex;
	}
	catch (...) {
		error = kCantHappenErr;
	}
	if (error) {
		if (error == kUnhandledMsgErr) {
			// Defined by Plugin.hpp and used in Plugin::Message - ignore.
			error = kNoErr;
		}
		else {
			Plugin::ReportError(error, caller, selector, message);
		}
	}
	return error;
}

ASErr ColorToolsPlugin::SetGlobal(Plugin *plugin)
{
    gPlugin = (ColorToolsPlugin *) plugin;
    return kNoErr;
}


ASErr ColorToolsPlugin::StartupPlugin( SPInterfaceMessage *message )
{
	ASErr error = kNoErr;
    error = Plugin::StartupPlugin(message);
    if (error) { return error; }
    
    error = this->AddMenus(message);
    if (error) { return error; }
    
    if (NULL == colorToolsUIController)
    {
        colorToolsUIController = new ColorToolsUIController();
        
        error = Plugin::LockPlugin(true);
        if (error) { return error; }
    }
    
    if (NULL == mySwatchList)
    {
        mySwatchList = new BtSwatchList();
    }
    
    //Register for notifiers
    error = sAINotifier->AddNotifier( fPluginRef, kColorToolsPluginName,
                                     kAICSXSPlugPlugSetupCompleteNotifier, &fRegisterEventNotifierHandle);
    if (error) { return error; }
    error = sAINotifier->AddNotifier( fPluginRef, kColorToolsPluginName,
                                     kAIApplicationStartedNotifier, &fAppStartedNotifierHandle);
    if (error) { return error; }
    error = sAINotifier->AddNotifier( fPluginRef, kColorToolsPluginName,
                                     kAIDocumentOpenedNotifier, &fDocOpenedNotifierHandle);
    if (error) { return error; }
    error = sAINotifier->AddNotifier( fPluginRef, kColorToolsPluginName,
                                     kAIArtCustomColorChangedNotifier, &fCustomColorChangeNotifierHandle);
    if (error) { return error; }
    error = sAINotifier->AddNotifier( fPluginRef, kColorToolsPluginName,
                                     kAISwatchLibraryChangedNotifier, &fSwatchLibChangeNotifierHandle);
    if (error) { return error; }
    error = sAINotifier->AddNotifier( fPluginRef, kColorToolsPluginName,
                                     kAIArtSelectionChangedNotifier, &fArtSelectionChangeNotifierHandle);
    if (error) { return error; }
    
    return error;
}

ASErr ColorToolsPlugin::ShutdownPlugin( SPInterfaceMessage *message )
{
    if (colorToolsUIController)
    {
        colorToolsUIController->RemoveEventListeners();
        delete colorToolsUIController;
        colorToolsUIController = NULL;
        Plugin::LockPlugin(false);
    }
    if (mySwatchList)
    {
        delete mySwatchList;
    }
    message->d.globals = NULL;
    return Plugin::ShutdownPlugin(message);

}

ASErr ColorToolsPlugin::ReloadPlugin(SPInterfaceMessage *message)
{
    return Plugin::ReloadPlugin(message);
}

ASErr ColorToolsPlugin::UnloadPlugin(SPInterfaceMessage *message)
{
    return Plugin::UnloadPlugin(message);
}

/*
 */
ASErr ColorToolsPlugin::PostStartupPlugin()
{
    return kNoErr;
}

/*
 */
ASErr ColorToolsPlugin::AddMenus(SPInterfaceMessage* message)
{
	AIErr error = kNoErr;
	
	// Add the menus.
	AIPlatformAddMenuItemDataUS menuData;
	AIPlatformAddMenuItemDataUS fixBlackMenuData;
	AIPlatformAddMenuItemDataUS addColorMenuData;
	AIPlatformAddMenuItemDataUS findReplaceMenuData;
	
	const char *menuGroupCStr = "SDKEditGroup";
	const char *menuCStr = "Modify Swatches";
	const char *fixBlackCStr = "Fix Black";
	const char *addColorCStr = "Add Color";
	const char *findReplaceCStr = "Find and Replace Graphics";
	AIMenuGroup	menuGroup;
	AIMenuGroup	pluginMenuGroup;
	AIMenuGroup dummyGroup;
	AIMenuItemHandle dummyItem;
	bool sdkGroupAlreadyMade = false;
	bool addColorGroupAlreadyMade = false;
	bool findReplaceGroupAlreadyMade = false;
	
	//This line tells AI to put the new item in the Edit menu
	menuData.groupName = kEditMenuGroup;
	menuData.itemText = ai::UnicodeString(menuCStr);
	
	fixBlackMenuData.groupName = menuGroupCStr;
	fixBlackMenuData.itemText = ai::UnicodeString(fixBlackCStr);
	
	addColorMenuData.groupName = menuGroupCStr;
	addColorMenuData.itemText = ai::UnicodeString(addColorCStr);
	
	findReplaceMenuData.groupName = menuGroupCStr;
	findReplaceMenuData.itemText = ai::UnicodeString(findReplaceCStr);
    
	
	// search through the menus and
	// see if another SDK plug in has already set up the group
	int count;
	error = sAIMenu->CountMenuGroups( &count );
	if ( error ) goto error;
	while (count > 0)
	{
		error = sAIMenu->GetNthMenuGroup( count-1, &dummyGroup );
		if ( error ) goto error;
		char *name;
		error = sAIMenu->GetMenuGroupName( dummyGroup, (const char**)&name );
		if ( error ) goto error;
		if ( std::strcmp(name, menuGroupCStr ) == 0 )
		{
			sdkGroupAlreadyMade = true;
			count = -1;
		}
		if(std::strcmp(name,addColorCStr) == 0)
		{
			addColorGroupAlreadyMade = true;
		}
		if(std::strcmp(name,findReplaceCStr) == 0)
		{
			findReplaceGroupAlreadyMade = true;
		}
		count--;
	}
	
	if ( !sdkGroupAlreadyMade )
	{
		error = sAIMenu->AddMenuItem( message->d.self, menuGroupCStr, &menuData, 0, &dummyItem );
		if ( error ) goto error;
		
		error = sAIMenu->AddMenuGroupAsSubMenu( menuGroupCStr, kMenuGroupSortedAlphabeticallyOption, dummyItem, &menuGroup );
		if ( error ) goto error;
	}
	
	
/*	if(!addColorGroupAlreadyMade)
	{
		error = sAIMenu->AddMenuItem( message->d.self, addColorCStr, &addColorMenuData, 0, &dummyItem );
		if ( error ) goto error;
		
		error = sAIMenu->AddMenuGroupAsSubMenu( addColorCStr, 0, dummyItem, &pluginMenuGroup );
		if ( error ) goto error;
	}
	
	if(!findReplaceGroupAlreadyMade)
	{
		error = sAIMenu->AddMenuItem( message->d.self, findReplaceCStr, &findReplaceMenuData, 0, &dummyItem );
		if ( error ) goto error;
		
		error = sAIMenu->AddMenuGroupAsSubMenu( findReplaceCStr, 0, dummyItem, &pluginMenuGroup );
		if ( error ) goto error;
	}
*/	
	
	error = sAIMenu->AddMenuItem( message->d.self, fixBlackCStr, &fixBlackMenuData, kMenuItemNoOptions, &FixBlackMenuItemSelected );
	if ( error ) goto error;
	
/*	addColorMenuData.groupName = addColorCStr;
	error = sAIMenu->AddMenuItem( message->d.self, addColorCStr, &addColorMenuData, kMenuItemWantsUpdateOption, &AddColorMenuItemSelected );
	if ( error ) goto error;
	
	findReplaceMenuData.groupName = findReplaceCStr;
	error = sAIMenu->AddMenuItem( message->d.self, findReplaceCStr, &findReplaceMenuData, kMenuItemWantsUpdateOption, &FindReplaceMenuItemSelected );
	if ( error ) goto error;
*/	
error:
	return error;
}

/*
 */
ASErr ColorToolsPlugin::GoMenuItem(AIMenuMessage* message)
{
	AIErr error = kNoErr;
	
	if ( message->menuItem == FixBlackMenuItemSelected )
	{
		//Call the main function
		if ( FixBlack() ) {
			//			sADMBasic->MessageAlert("Black was fixed.");
			//		} else {
			//			sADMBasic->MessageAlert("Black was not fixed.");
		}
	}
	if ( message->menuItem == AddColorMenuItemSelected )
	{
        //**Deprecated
		//sADMDialog->Show( g->AddColorDialog, !( sADMDialog->IsVisible( g->AddColorDialog ) ) );
	}
	if ( message->menuItem == FindReplaceMenuItemSelected )
	{
        //**Deprecated
		//sADMDialog->Show( g->FindReplaceDialog, !( sADMDialog->IsVisible( g->FindReplaceDialog ) ) );
	}
	
	if (error)
		goto error;
	
error:
	return error;
}


ASErr ColorToolsPlugin::UpdateMenuItem(AIMenuMessage* message)
{
	//	long inArtwork;
	//	long isSelected;
	//	long isTrue;
	AIErr error = kNoErr;
	
    /*Deprecated
	if (message->menuItem == AddColorMenuItemSelected) {
		if ( sADMDialog->IsVisible( g->AddColorDialog ) )
		{
			sAIMenu->SetItemText( message->menuItem, ai::UnicodeString("Hide Add Color") );
		}
		else
		{
			sAIMenu->SetItemText( message->menuItem, ai::UnicodeString("Show Add Color") );
		}
	}
	if (message->menuItem == FindReplaceMenuItemSelected) {
		if ( sADMDialog->IsVisible( g->FindReplaceDialog ) )
		{
			sAIMenu->SetItemText( message->menuItem, ai::UnicodeString("Hide Find and Replace Graphics") );
		}
		else
		{
			sAIMenu->SetItemText( message->menuItem, ai::UnicodeString("Show Find and Replace Graphics") );
		}
	}
	*/
	
	
	/*
	 // This is only valid when kSelectorAIMacUpdateMenuItem is received, which is why we're here
	 error = sAIMenu->GetUpdateFlags( &inArtwork, &isSelected, &isTrue );
	 if ( error )
	 goto error;
	 
	 // We get this here because the menu group into which we are installing automatically alphabetizes
	 error = sAIMenu->GetPlatformMenuItem( g->manualUpdateAIMenu, &pmanualMenuItem );
	 if ( error )
	 goto error;
	 
	 // Do something based on the result.  It's easier than GetMatchingArt for this example, and can
	 // also be used for a quick check before more processing.
	 if ( isSelected & kMacIfGuide ) {
	 sMacMenu->MacSetItem( pmanualMenuItem.menu, pmanualMenuItem.item, "\pDo Something To Selected Guides" );
	 }
	 else if ( inArtwork & kMacIfGuide ) {
	 sMacMenu->MacSetItem( pmanualMenuItem.menu, pmanualMenuItem.item, "\pDo Something To All Guides" );
	 }
	 */
	
	if (error)
		goto error;
	
error:
	return error;
}

ASErr ColorToolsPlugin::Notify(AINotifierMessage *message )
{
    if ( message->notifier == fRegisterEventNotifierHandle)
    {
        colorToolsUIController->RegisterCSXSEventListeners();
    }
    
    if ( message->notifier == fAppStartedNotifierHandle ) {
        // Whatever we want to do when the app starts
    }
    
    if (message->notifier == fDocOpenedNotifierHandle ||
        message->notifier == fCustomColorChangeNotifierHandle ||
        message->notifier == fSwatchLibChangeNotifierHandle ) {
        //TODO: Update the color lists
    }
    
    if (message->notifier == fArtSelectionChangeNotifierHandle ) {
        colorToolsUIController->DetermineChangeInStatus();
    }
    return kNoErr;
}
