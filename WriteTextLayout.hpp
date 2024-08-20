#pragma once
#include "Font.hpp"

class WriteTextLayout
{
public:
	struct ColourRange
	{
		D2D1_COLOR_F colour;
		DWRITE_TEXT_RANGE range;
	};

	struct FontRange
	{
		Font font;
		DWRITE_TEXT_RANGE range;
	};

	using ColourRanges = std::vector<ColourRange>;
	using FontRanges = std::vector<FontRange>;

	static ColourRanges parse_tf_colours(std::wstring_view text);
	static FontRanges parse_tf_fonts(std::wstring_view text);
	static HRESULT create(wil::com_ptr_t<IDWriteTextLayout>& text_layout, const Font& font, const FormatParams& params, std::wstring_view text, float width, float height);
	static HRESULT create3(wil::com_ptr_t<IDWriteTextLayout3>& text_layout, const Font& font, const FormatParams& params, std::wstring_view text);
	static HRESULT set_colours(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, const ColourRanges& colour_ranges);
	static HRESULT set_colours_json(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, JSON& jcolours);
	static HRESULT set_fonts(IDWriteTextLayout* text_layout, const FontRanges& font_ranges);
	static HRESULT set_fonts_json(IDWriteTextLayout* text_layout, JSON& jfonts);
	static HRESULT set_line_spacing(IDWriteTextLayout3* text_layout);
	static float calc_text_width(std::wstring_view text, const Font& font);

private:
	static HRESULT to_range(JSON& obj, DWRITE_TEXT_RANGE& range, bool verify_colour = false);
};
