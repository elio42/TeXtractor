# Gen-AI prompts

## 1. General complex prompt:

The user wants to extract (OCR) all the text from the attached image.

### Tasks

1. **Analyze**
   First, decide which of the following types best describes the image content:
   1. Simple text block with or without formulas.
   2. Math expressions where formulas make up more than 90% of the content.
   3. Hard‑to‑read photo (e.g., low resolution, skewed, noisy, or cluttered).

2. **Transcribe**
   - Extract all visible textual content into Markdown/LaTeX, following the rules for the chosen type.  
   - Output **only** the transcription, with no explanations or comments.  
   - Replace any graphs, charts, or non‑text diagrams with a placeholder of the form:  
     `[placeholder: short description of the graph]`  
   - For LaTeX formulas, prefer simpler, more common symbols unless a more specific variant is clearly required for correctness.  
     - For example, use `\hat{a}` instead of `\widehat{a}` when both would be acceptable.  
     - Use non‑bold letters by default unless the original clearly uses bold.

### Type‑specific rules

**1. Simple text block with or without formulas**

- Transcribe all text faithfully, preserving paragraphs and basic structure (headings, lists) when possible.  
- Write any math expressions in valid LaTeX.  
- Choose inline math (`$…$`) or display math (`$$…$$`) based on how the expression appears in the image:
  - Inline if it appears inside a line of text.
  - Display if it appears centered on its own line or clearly separated as a formula.

**2. Math expressions (formulas > 90%)**

- Transcribe all formulas in LaTeX, using **display mode** (`$$…$$`) for each visible formula.  
- Ignore stray letters, UI elements, or words that are clearly unrelated to the mathematical content (for example, window titles, button labels, status bar text, etc...).

**3. Hard‑to‑read photos**

- Transcribe any text you can reasonably read.  
- Still use Markdown formatting (paragraphs, bullet lists, headings) where it improves readability, even if this does not exactly match the original layout.  
- If a part of the text is illegible, mark it as `[ILLEGIBLE]` rather than guessing.

## 2. General simple prompt:

The user wants to extract (OCR) all the text from the attached image.

Provide the extracted text in Markdown format, preserving basic structure (headings, lists) when possible.

Provide mathematical formulas in LaTeX format where applicable.

Output **only** the transcription, with no explanations or comments.