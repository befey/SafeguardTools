
// Tell Xcode to export the following symbols
#if defined(__GNUC__)
#pragma GCC visibility push(default)
#endif

#include "ColorFuncs.h"
#include "stdio.h"
#include "FixBlack.h"
#include "SafeguardToolsSuites.h"
#include "BtSwatchList.h"
#include "SafeguardToolsPlugin.h"

bool FixBlack() {

	//Set the undo/redo text
	sAIUndo->SetUndoTextUS(ai::UnicodeString("Undo Fix Black"), ai::UnicodeString("Redo Fix Black"));
	
	AIBoolean converted = FALSE;
	sAIDocument->SetDocumentSpotColorMode(NULL, kAILegacySpotColorMode, TRUE, &converted);
    
    
    gPlugin->GetBtSwatchList()->CreateOrConvertToCustomColor(BLACK_COLOR_NAME);
    gPlugin->GetBtSwatchList()->CreateOrConvertToCustomColor(WHITE_COLOR_NAME);

    gPlugin->GetBtSwatchList()->AdjustAllColors();
	
	// 4. Name all the colors
    VisitAIColorFlags controlFlags = kVisitColorsSolidOnly | kVisitGlobalObjectsOnceOnly;
	sAIPathStyle->AdjustObjectAIColors( NULL , nameAllColors , NULL , controlFlags , NULL );
	
	// 5. Remove unused colors
	gPlugin->GetBtSwatchList()->RemoveUnusedColors();
	
	return TRUE;
}