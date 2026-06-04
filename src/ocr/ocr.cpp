#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <stdexcept>

#include "../core/settings.h"

std::string getOCR(const std::string &path){
    char *outText = nullptr;
    Settings settings;

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    if (api->Init(NULL, settings.getOCRLanguage().c_str())) {
        delete api;
        throw std::runtime_error("Could not initialize tesseract.");
    }

    Pix *image = pixRead(path.c_str());
    if (!image) {
        api->End();
        delete api;
        throw std::runtime_error("Could not open image for OCR.");
    }

    api->SetImage(image);
    outText = api->GetUTF8Text();
    if (!outText) {
        api->End();
        delete api;
        pixDestroy(&image);
        throw std::runtime_error("Tesseract returned empty OCR buffer.");
    }

    std::string result = outText;
    delete[] outText;

    api->End();
    delete api;
    pixDestroy(&image);
    return result;
}