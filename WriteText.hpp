#pragma once

class WriteText
{
public:
	static HRESULT apply_alignment_and_trimming(IDWriteTextFormat* text_format, uint32_t text_alignment, uint32_t paragraph_alignment, uint32_t word_wrapping, uint32_t trimming_granularity);
	static HRESULT apply_font(IDWriteTextLayout* text_layout, JSON& font, DWRITE_TEXT_RANGE range);
	static HRESULT apply_fonts(IDWriteTextLayout* text_layout, JSON& fonts);
	static HRESULT create_format(wil::com_ptr_t<IDWriteTextFormat>& text_format, JSON& font);
	static HRESULT create_format(wil::com_ptr_t<IDWriteTextFormat>& text_format, std::wstring_view name = factory::DefaultFont, float size = 16.f, uint32_t weight = 400U, uint32_t style = 0U, uint32_t stretch = 5U);
	static HRESULT create_layout(wil::com_ptr_t<IDWriteTextLayout>& text_layout, IDWriteTextFormat* text_format, std::wstring_view text, float width = FLT_MAX, float height = FLT_MAX);
	static HRESULT to_range(JSON& obj, DWRITE_TEXT_RANGE& range);

private:
	static BOOL get_BOOL(JSON& obj);

	template <typename T>
	static T get(JSON& obj);
};
