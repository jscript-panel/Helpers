#include "stdafx.hpp"
#include "WriteText.hpp"

BOOL WriteText::get_BOOL(JSON& obj)
{
	return obj.get<bool>() ? TRUE : FALSE;
}

HRESULT WriteText::apply_alignment(IDWriteTextLayout* text_layout, uint32_t text_alignment, uint32_t paragraph_alignment, uint32_t word_wrapping)
{
	const auto dtext_alignment = static_cast<DWRITE_TEXT_ALIGNMENT>(text_alignment);
	const auto dparagraph_alignment = static_cast<DWRITE_PARAGRAPH_ALIGNMENT>(paragraph_alignment);
	const auto dword_wrapping = static_cast<DWRITE_WORD_WRAPPING>(word_wrapping);
	RETURN_IF_FAILED(text_layout->SetTextAlignment(dtext_alignment));
	RETURN_IF_FAILED(text_layout->SetParagraphAlignment(dparagraph_alignment));
	RETURN_IF_FAILED(text_layout->SetWordWrapping(dword_wrapping));
	return S_OK;
}

HRESULT WriteText::apply_font(IDWriteTextLayout* text_layout, JSON& font, DWRITE_TEXT_RANGE range)
{
	auto& name = font["Name"];
	if (name.is_string())
	{
		const std::wstring tmp = to_wide(name.get<std::string>());
		RETURN_IF_FAILED(text_layout->SetFontFamilyName(tmp.data(), range));
	}

	auto& size = font["Size"];
	if (size.is_number_unsigned())
	{
		const auto tmp = get<float>(size);
		RETURN_IF_FAILED(text_layout->SetFontSize(tmp, range));
	}

	auto& weight = font["Weight"];
	if (weight.is_number_unsigned())
	{
		const auto tmp = get<DWRITE_FONT_WEIGHT>(weight);
		RETURN_IF_FAILED(text_layout->SetFontWeight(tmp, range));
	}

	auto& style = font["Style"];
	if (style.is_number_unsigned())
	{
		const auto tmp = get<DWRITE_FONT_STYLE>(style);
		RETURN_IF_FAILED(text_layout->SetFontStyle(tmp, range));
	}

	auto& stretch = font["Stretch"];
	if (stretch.is_number_unsigned())
	{
		const auto tmp = get<DWRITE_FONT_STRETCH>(stretch);
		RETURN_IF_FAILED(text_layout->SetFontStretch(tmp, range));
	}

	auto& strikethrough = font["Strikethrough"];
	if (strikethrough.is_boolean())
	{
		const auto tmp = get_BOOL(strikethrough);
		RETURN_IF_FAILED(text_layout->SetStrikethrough(tmp, range));
	}

	auto& underline = font["Underline"];
	if (underline.is_boolean())
	{
		const auto tmp = get_BOOL(underline);
		RETURN_IF_FAILED(text_layout->SetUnderline(tmp, range));
	}
	return S_OK;
}

HRESULT WriteText::apply_font_or_fonts(IDWriteTextLayout* text_layout, wil::zwstring_view font_string)
{
	if (font_string.empty()) return S_OK;

	auto font = JSONHelper::parse(font_string);
	if (font.is_object())
	{
		const auto range = DWRITE_TEXT_RANGE(0U, UINT_MAX);
		RETURN_IF_FAILED(apply_font(text_layout, font, range));
	}
	else if (font.is_array())
	{
		RETURN_IF_FAILED(apply_fonts(text_layout, font));
	}
	return S_OK;
}

HRESULT WriteText::apply_fonts(IDWriteTextLayout* text_layout, JSON& fonts)
{
	for (auto&& font : fonts)
	{
		DWRITE_TEXT_RANGE range{};
		RETURN_IF_FAILED(JSONHelper::to_dwrite_text_range(font, range));
		RETURN_IF_FAILED(apply_font(text_layout, font, range));
	}
	return S_OK;
}

HRESULT WriteText::apply_trimming(IDWriteTextFormat* text_format, uint32_t trimming_granularity)
{
	DWRITE_TRIMMING trimming{};
	trimming.granularity = static_cast<DWRITE_TRIMMING_GRANULARITY>(trimming_granularity);

	wil::com_ptr_t<IDWriteInlineObject> trimmingSign;
	RETURN_IF_FAILED(g_dwrite_factory->CreateEllipsisTrimmingSign(text_format, &trimmingSign));
	RETURN_IF_FAILED(text_format->SetTrimming(&trimming, trimmingSign.get()));
	return S_OK;
}

HRESULT WriteText::create_format(wil::com_ptr_t<IDWriteTextFormat>& text_format, uint32_t trimming_granularity)
{
	RETURN_IF_FAILED(create_format(text_format));
	RETURN_IF_FAILED(apply_trimming(text_format.get(), trimming_granularity));
	return S_OK;
}

HRESULT WriteText::create_format(wil::com_ptr_t<IDWriteTextFormat>& text_format, wil::zwstring_view name, float size, uint32_t weight, uint32_t style, uint32_t stretch)
{
	const auto dweight = static_cast<DWRITE_FONT_WEIGHT>(weight);
	const auto dstyle = static_cast<DWRITE_FONT_STYLE>(style);
	const auto dstretch = static_cast<DWRITE_FONT_STRETCH>(stretch);
	return g_dwrite_factory->CreateTextFormat(name.data(), nullptr, dweight, dstyle, dstretch, size, L"", &text_format);
}

HRESULT WriteText::create_layout(wil::com_ptr_t<IDWriteTextLayout>& text_layout, IDWriteTextFormat* text_format, wil::zwstring_view text, float width, float height)
{
	return g_dwrite_factory->CreateTextLayout(text.data(), to_uint(text.length()), text_format, width, height, &text_layout);
}

template <typename T>
T WriteText::get(JSON& obj)
{
	return static_cast<T>(obj.get<uint32_t>());
}
