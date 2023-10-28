#pragma once

HRESULT apply_alignment(IDWriteTextLayout* text_layout, uint32_t text_alignment, uint32_t paragraph_alignment, uint32_t word_wrapping);
HRESULT apply_font(IDWriteTextLayout* text_layout, JSON& j, DWRITE_TEXT_RANGE range);
HRESULT apply_fonts(IDWriteTextLayout* text_layout, wil::zwstring_view font_string);
HRESULT apply_trimming(IDWriteTextFormat* text_format, uint32_t trimming_granularity);
