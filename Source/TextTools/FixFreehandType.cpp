/*
 *  FixFreehandType.cpp
 *  TextTools
 *
 *  Created by Comp Mac 8a on 11/4/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "ATEFuncs.h"
#include "FixFreehandType.h"

bool FixFreehandType() {
	//Set the undo/redo text
    sAIUndo->SetUndoTextUS(ai::UnicodeString("Undo Fix Freehand Type"), ai::UnicodeString("Redo Fix Freehand Type"));
	
	int docCount = 0;
	sAIDocumentList->Count(&docCount);
	if(docCount > 0) {
		//Create an art set of only the selected text objects
		//CREATE THE ART SET
		AIArtSet artSet;
		sAIArtSet->NewArtSet(&artSet);
		AIArtSpec selectedSpecs[] = { { kTextFrameArt , kArtFullySelected , kArtFullySelected } };
		sAIArtSet->MatchingArtSet( selectedSpecs , 1, artSet );
		
		//Loop the art set
		size_t count;
        sAIArtSet->CountArtSet( artSet, &count );
		for ( int i=0 ; i < count ; i++ ) {
			
			AIArtHandle currArtHandle = NULL;
			sAIArtSet->IndexArtSet( artSet, i, &currArtHandle );
			
			AITextFrameType type;
			sAITextFrame->GetType(currArtHandle, &type);
			//We only want in path and point type objects
			if (type == kInPathTextType || type == kPointTextType) {
												
				//Create an ITextFrame object from the original TextFrame art
				TextFrameRef origATETextFrameRef;
				sAITextFrame->GetATETextFrame(currArtHandle, &origATETextFrameRef);
				ATE::ITextFrame origITextFrame(origATETextFrameRef);
				
				//Find the angle of rotation
				AIRealMatrix origMatrix = origITextFrame.GetMatrix();
				AIReal angle;
				sAIRealMath->AIRealMatrixGetRotate(&origMatrix, &angle);
				
				//Remove any rotation
				//AIRealRect convertedToPathsBounds, unRotateBounds;
				AIRealMatrix unrotateMatrix;
				sAIRealMath->AIRealMatrixSetIdentity(&unrotateMatrix);
				sAIRealMath->AIRealMatrixConcatRotate(&unrotateMatrix, angle);
				sAITransformArt->TransformArt(currArtHandle, &unrotateMatrix, 0, kTransformObjects);
				
				//Create the ATE range
				ATE::TextRangeRef currATETextRangeRef;
				sAITextFrame->GetATETextRange(currArtHandle, &currATETextRangeRef);
				ATE::ITextRange currTextRange(currATETextRangeRef);
				
				//We'll store the baseline shift if the first run, so we can modify all the baseline shifts
				//back to 0
				bool isAssigned;
				ATE::ITextRunsIterator it = currTextRange.GetTextRunsIterator();
				ATE::ICharFeatures currCharFeatures = it.Item().GetUniqueCharFeatures();
				ASReal firstRunBaselineShift = currCharFeatures.GetBaselineShift(&isAssigned);
				
				//Now move the whole text object by the baseline shift, so when we set it to 0,
				//the text falls in the same place
				AIRealMatrix matrix;
				sAIRealMath->AIRealMatrixSetIdentity(&matrix);
				sAIRealMath->AIRealMatrixSetTranslate(&matrix, 0, firstRunBaselineShift);
				sAITransformArt->TransformArt(currArtHandle, &matrix, 0, kTransformObjects);
				
				//Apply the rotation again
				sAIRealMath->AIRealMatrixSetIdentity(&matrix);
				sAIRealMath->AIRealMatrixConcatRotate(&matrix, -angle);
				sAITransformArt->TransformArt(currArtHandle, &matrix, 0, kTransformObjects);
				
				FixTextAttributes(currTextRange, firstRunBaselineShift);
			}
            
            sAIArtSet->DisposeArtSet(&artSet);
		}
	}
	
	return true;
}

bool FixTextAttributes(ATE::ITextRange theRange, ASReal firstRunBaselineShift) {
	bool isAssigned = FALSE;
	ATE::ITextRunsIterator it = theRange.GetTextRunsIterator();
	
	//If there is stuff to iterate through
	if ( !it.IsEmpty() ) {
		
		bool isFirst = TRUE;
		bool foundGoodFeatures = FALSE;
		ATE::ICharFeatures knownGoodCharFeatures;
		
		ATE::ICharFeatures currCharFeatures;
		
		while (it.IsNotDone()) {
			
			//Loop through all the ranges in the run
			while (it.IsNotDone()) {
				
				//If this is the first range, we need to go through the run and find the first good fill or stroke color
				if (isFirst) {
					while ( foundGoodFeatures == FALSE ) {
						if (it.IsNotDone()) {
							knownGoodCharFeatures = it.Item().GetUniqueCharFeatures();
							
							bool fillAssigned = FALSE;
							bool strokeAssigned = FALSE;
							knownGoodCharFeatures.GetFillColor(&fillAssigned);
							knownGoodCharFeatures.GetStrokeColor(&strokeAssigned);
							
							if (fillAssigned || strokeAssigned) {
								foundGoodFeatures = TRUE;
							}
						}
						it.Next();
					}
					//Put our iterator back to the beginning again
					it.MoveToFirst();
				}
				
				//Now we'll check the color of our current range
				currCharFeatures = it.Item().GetUniqueCharFeatures();
				bool currFillAssigned = FALSE;
				bool currStrokeAssigned = FALSE;
				currCharFeatures.GetFillColor(&currFillAssigned);
				currCharFeatures.GetStrokeColor(&currStrokeAssigned);
								
				//If it has a valid color, we'll store that for the next time
				
				if ( currFillAssigned || currStrokeAssigned) {
					knownGoodCharFeatures = currCharFeatures;
									
					isFirst = FALSE;
				} else {
					//Otherwise, we'll set the color of the range to the known good color
					it.Item().SetLocalCharFeatures(knownGoodCharFeatures);
				}
				
				//Also, we need to adjust the baselineshift here
				ATE::ICharFeatures baselineShiftFeatures(knownGoodCharFeatures);
				ASReal currBaselineShift = currCharFeatures.GetBaselineShift(&isAssigned);
				currBaselineShift -= firstRunBaselineShift;
				baselineShiftFeatures.SetBaselineShift(currBaselineShift);
				it.Item().SetLocalCharFeatures(baselineShiftFeatures);
				
				//Move to the next run
				it.Next();
			}
		}
	}
    return isAssigned;
}
