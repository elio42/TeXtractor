# TeXtractor

TeXtractor is a Linux desktop application to extract **any** text from an image. TeXtractor uses both traditional OCR and (optionally) multi-modal AI models for text and LaTeX formula extraction. You can use both online models and local models using ollama.

On the surface, TeXtractor is an application that can be opened with an image and it will extract the text for you. The real intention however, is for TeXtractor to be used with your screenshot tool. Some screenshot applications like KDE Spectacle have an `export` button that you can use to open the just taken screenshot in an external application like TeXtractor.

This way you get the best of both worlds. Neither a tool that pops up on every screenshot which might be in the way like "spectacle-ocr-screenshot" (because it opens on every screenshot). But you only need two clicks to actually get your OCR result if you _DO_ want to extract text from a screenshot. Furthermore,  it's a simple and straightforward application if you ever need to extract text from any other image.

## Usage

- Launch without arguments to open the Home page with settings.
- Launch with an image path argument to immediately run OCR on that image.
- In the Results view, use the provider dropdown and `Extract with AI` button to run AI extraction if requested.

Examples:

```bash
./build/TeXtractor
./build/TeXtractor /path/to/image.png
```

Or, of course, use the "export" button in your screenshot tool to launch TeXtractor.

### Model selection

Any multimodal AI model can be used. Here is a selection of models that I tested and the results I got:

#### Google Gemini - gemini-3-flash-preview

Pro:
- This performs by far the best and the fastest.
- Google's free-tier is usually plenty for this use case here.
  - (Rate limits of 5 requests/minute but no monthly limits.) (January 2026)

Cons:
- Google uses your images for training when using the free tier.

#### Ollama - gemma4:e4b

Pro:
- Delivers good results
- Runs on my 6-year old laptop with a gtx 1650ti

Cons:
- Takes longer on older devices and **especially** with large images.
- May not be suitable if you don't have a dedicated GPU (though I didn't do any testing without a dedicated GPU so your experience may vary)

## Building and Install

Build dependencies (Linux):

- C++ compiler with C++17 support
- CMake 3.16+
- Qt6 Widgets + Concurrent
- Tesseract + Leptonica development libraries
- libcurl development library

**Build:**

```bash
cmake -B build
cmake --build build --parallel
```

**Run:**

```bash
./build/TeXtractor
```

### Install with cmake - mostly for testing purposes

```bash
sudo cmake --install build
```

```bash
cd build
sudo xargs rm < install_manifest.txt
# or in case the manifest is gone check here:
sudo rm /usr/bin/textractor
sudo rm /usr/share/applications/textractor.desktop
sudo rm /usr/share/icons/hicolor/256x256/apps/textractor.png
```
