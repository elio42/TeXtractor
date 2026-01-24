#pragma once
#include <string>

namespace Prompts {
    const std::string default_prompt = R"(
    Extract the Text from the image accurately, preserving mathematical notation and formatting.
    - Provide the output in LaTeX format where applicable.
    - Do not include any explanations or additional text; only return the LaTeX code.
    )";
    
}