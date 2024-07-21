#pragma once
#include "Font.hpp"
#include "FormatParams.hpp"

class WriteText
{
public:
	static HRESULT apply_colours(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, JSON& colours);
	static HRESULT apply_colours(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, std::wstring_view text);
	static HRESULT apply_fonts(IDWriteTextLayout* text_layout, JSON& jfonts);
	static HRESULT apply_fonts(IDWriteTextLayout* text_layout, std::wstring_view text);
	static HRESULT create_format(wil::com_ptr_t<IDWriteTextFormat>& text_format, const Font& font, const FormatParams& params = {});
	static HRESULT create_layout(wil::com_ptr_t<IDWriteTextLayout>& text_layout, IDWriteTextFormat* text_format, std::wstring_view text, float width = FLT_MAX, float height = FLT_MAX);

private:
	static DWRITE_TEXT_RANGE to_range(size_t start, size_t length);
	static HRESULT to_range(JSON& obj, DWRITE_TEXT_RANGE& range, bool verify_colour = false);
	static HRESULT apply_colour(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, const D2D1_COLOR_F& colour, const DWRITE_TEXT_RANGE& range);
	static HRESULT apply_font(IDWriteTextLayout* text_layout, const Font& font, const DWRITE_TEXT_RANGE& range);
};
