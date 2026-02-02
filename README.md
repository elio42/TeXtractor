unfinished README draft:

# TeXtractor

TeXtractor is a Linux desktop application to extract **any** text from an image. TeXtractor uses both traditional OCR and multi modal AI models for text and LaTeX formula extraction. You can use both online models and local models with ollama.

On the surface, TeXtractor is an application that can be opened with an image and it will extract the text for you. The real intention however, is for TeXtractor to be used with your screenshot tool. Some screenshot applications like KDE Spectacle have an `export` button that you can use to open TeXtractor with the just taken screenshot.

## Usage

write something...

### Model selection

Any multimodel AI model can be used. Here is a selection of model that I tested and the results I got:

#### Google Gemini

Pro:
- This performs by far the best and the fastest.
- Google offers a generous free-tier for their API.
  - (Rate limits of 5 requests/minute but no monthly limits.) (January 2026)

Cons:
- Google uses your images for training.

#### qwen3-vl:8b (Ollama)

Pro:
- Performs the best from the local models tested.
  - Extracts even complext LaTeX formulas with less common operators correctly.
  - Rivals Google Gemini in accuracy
- Uses only ~6Gb of VRAM/RAM so it will fit most GPU's

Cons:
- Ungodly slow.
- Sometimes doesn't even manage to finish with large images. (I let is run for over 20 min for some of the larger test images...)

#### gemma3:12b (Ollama) (8.1 Gb)

Pro:
- Text extraction works quite well including formatting.
- Order of magnitude faster than Qwen3.

Cons:
- Struggles with complex LaTeX formulas
  - Repeatedly misclassified more complex operators like substacks.

With a size of just over 8Gb it will fit most Laptops with unified memory but it might not fit into the VRAM of most dedicated GPU's (Many of which still have 8Gb).

#### gemma3:4b (Ollama) (3.3 Gb)

Pros:
- Very fast
- Fits into just 3Gb of VRAM/RAM

Cons:
- Usually not much better than traditional OCR so quite useless.

#### Table comparing the time to answer for different machines

- XPS15 9500 (2020): i7-10750H, GTX 1650 Ti mobile
- Asus Zenbook (____): 
- Pc: I7 _____, Radeon RX 9070 XT

Time \ System | Pc | XPS15 | Zenbook
-|-|-|-
Gemini | 15s | 15s | 15s
qwen3-vl:8b
gemma3:4b
gemma3:12b

## Building and Install

write something one day...