#pragma once
#include <string>

namespace Prompts {
    const std::string default_prompt = R"(
    Extract the Text from the image accurately, preserving mathematical notation and formatting.
    - Provide the output in LaTeX format where applicable.
    - Do not include any explanations or additional text; only return the LaTeX code.
    )";
    const std::string gemeini_prompt = R"(The user wanto to extract text from an image. 
- Please provide the extracted text in Markdown format
- Preserve all mathematical notation and formatting using LaTeX syntax.
- Do not include any explanations or additional text; only return the Markdown with embedded LaTeX
})";
}