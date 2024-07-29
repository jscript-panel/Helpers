#pragma once
#include "Font.hpp"

class WriteTextLayout
{
public:
	static HRESULT apply_colours(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, JSON& colours);
	static HRESULT apply_colours(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, std::wstring_view text);
	static HRESULT apply_fonts(IDWriteTextLayout* text_layout, JSON& jfonts);
	static HRESULT apply_fonts(IDWriteTextLayout* text_layout, std::wstring_view text);
	static HRESULT apply_line_spacing(IDWriteTextLayout3* text_layout);
	static HRESULT create(wil::com_ptr_t<IDWriteTextLayout>& text_layout, const Font& font, const FormatParams& params, std::wstring_view text, float width, float height);
	static HRESULT create3(wil::com_ptr_t<IDWriteTextLayout3>& text_layout, const Font& font, const FormatParams& params, std::wstring_view text);

private:
	static HRESULT to_range(JSON& obj, DWRITE_TEXT_RANGE& range, bool verify_colour = false);
	static HRESULT apply_colour(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, const D2D1_COLOR_F& colour, const DWRITE_TEXT_RANGE& range);
	static HRESULT apply_font(IDWriteTextLayout* text_layout, const Font& font, const DWRITE_TEXT_RANGE& range);
};
