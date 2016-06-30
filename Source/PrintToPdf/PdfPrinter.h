//
//  PdfPrinter.h
//  SafeguardTools
//
//  Created by T431962 on 4/4/16.
//
//

#ifndef __SafeguardTools__PdfPrinter__
#define __SafeguardTools__PdfPrinter__

#include "PdfSettings.h"
#include "PdfResults.h"
#include "PasswordRetriever.h"
#include "PathBuilder.h"
#include "ExistingFileDeleter.h"

namespace PrintToPdf
{
    class PdfPrinter
    {
    public:
        static unique_ptr<PdfPrinter> GetPrinter(shared_ptr<PdfSettings> settings);
        
        PdfResults Print() const;
        
    protected:
        PdfPrinter(shared_ptr<PdfSettings> settings);
        
        shared_ptr<PdfSettings> pdfSettings;
        unique_ptr<PasswordRetriever> pwRetriever;
        unique_ptr<PathBuilder> pathBuilder;
        unique_ptr<ExistingFileDeleter> efDeleter;
   
        const PlateNumber GetPlateNumber() const;
        const string GetToken(int plateIndex) const;
        
    private:
        virtual PdfResults CustomPrintSteps(ai::FilePath pathToPdfFile) const = 0;
    };
    
    class SingleFilePdfPrinter : public PdfPrinter
    {
    public:
        SingleFilePdfPrinter(shared_ptr<PdfSettings> settings);

        PdfResults CustomPrintSteps(ai::FilePath pathToPdfFile) const override;
    };
    
    class SeparateFilePdfPrinter : public PdfPrinter
    {
    public:
        SeparateFilePdfPrinter(shared_ptr<PdfSettings> settings);

        PdfResults CustomPrintSteps(ai::FilePath pathToPdfFile) const;
    };
}
#endif /* defined(__SafeguardTools__PrintToPdf__) */
