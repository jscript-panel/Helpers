#pragma once
#include "Font.hpp"

struct FormatParams
{
	FormatParams(uint32_t text_alignment, uint32_t paragraph_alignment, uint32_t word_wrapping, uint32_t trimming_granularity)
		: m_text_alignment(static_cast<DWRITE_TEXT_ALIGNMENT>(text_alignment))
		, m_paragraph_alignment(static_cast<DWRITE_PARAGRAPH_ALIGNMENT>(paragraph_alignment))
		, m_word_wrapping(static_cast<DWRITE_WORD_WRAPPING>(word_wrapping))
	{
		m_trimming.granularity = static_cast<DWRITE_TRIMMING_GRANULARITY>(trimming_granularity);
	}

	DWRITE_TEXT_ALIGNMENT m_text_alignment{};
	DWRITE_PARAGRAPH_ALIGNMENT m_paragraph_alignment{};
	DWRITE_WORD_WRAPPING m_word_wrapping{};
	DWRITE_TRIMMING m_trimming{};
};

class WriteText
{
public:
	static HRESULT apply_colours(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, JSON& colours);
	static HRESULT apply_colours(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, std::wstring_view text);
	static HRESULT apply_fonts(IDWriteTextLayout* text_layout, JSON& jfonts);
	static HRESULT apply_fonts(IDWriteTextLayout* text_layout, std::wstring_view text);
	static HRESULT create_format(wil::com_ptr_t<IDWriteTextFormat>& text_format, const Font& font = {});
	static HRESULT create_format(wil::com_ptr_t<IDWriteTextFormat>& text_format, const Font& font, const FormatParams& params);
	static HRESULT create_layout(wil::com_ptr_t<IDWriteTextLayout>& text_layout, IDWriteTextFormat* text_format, std::wstring_view text, float width = FLT_MAX, float height = FLT_MAX);

private:
	static DWRITE_TEXT_RANGE to_range(size_t start, size_t length);
	static HRESULT to_range(JSON& obj, DWRITE_TEXT_RANGE& range, bool verify_colour = false);
	static HRESULT apply_colour(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, const D2D1_COLOR_F& colour, const DWRITE_TEXT_RANGE& range);
	static HRESULT apply_font(IDWriteTextLayout* text_layout, const Font& font, const DWRITE_TEXT_RANGE& range);
	static HRESULT apply_format_params(IDWriteTextFormat* text_format, const FormatParams& params);
};
