#include "stdafx.hpp"
#include "WriteText.hpp"

HRESULT apply_alignment(IDWriteTextLayout* text_layout, uint32_t text_alignment, uint32_t paragraph_alignment, uint32_t word_wrapping)
{
	RETURN_IF_FAILED(text_layout->SetTextAlignment(static_cast<DWRITE_TEXT_ALIGNMENT>(text_alignment)));
	RETURN_IF_FAILED(text_layout->SetParagraphAlignment(static_cast<DWRITE_PARAGRAPH_ALIGNMENT>(paragraph_alignment)));
	RETURN_IF_FAILED(text_layout->SetWordWrapping(static_cast<DWRITE_WORD_WRAPPING>(word_wrapping)));
	return S_OK;
}

HRESULT apply_font(IDWriteTextLayout* text_layout, JSON& j, DWRITE_TEXT_RANGE range)
{
	auto& name = j["Name"];
	if (name.is_string())
	{
		const std::wstring tmp = to_wide(name.get<std::string>());
		RETURN_IF_FAILED(text_layout->SetFontFamilyName(tmp.data(), range));
	}

	auto& size = j["Size"];
	if (size.is_number_unsigned())
	{
		const auto tmp = static_cast<float>(size.get<uint32_t>());
		RETURN_IF_FAILED(text_layout->SetFontSize(tmp, range));
	}

	auto& weight = j["Weight"];
	if (weight.is_number_unsigned())
	{
		const auto tmp = static_cast<DWRITE_FONT_WEIGHT>(weight.get<uint32_t>());
		RETURN_IF_FAILED(text_layout->SetFontWeight(tmp, range));
	}

	auto& style = j["Style"];
	if (style.is_number_unsigned())
	{
		const auto tmp = static_cast<DWRITE_FONT_STYLE>(style.get<uint32_t>());
		RETURN_IF_FAILED(text_layout->SetFontStyle(tmp, range));
	}

	auto& stretch = j["Stretch"];
	if (stretch.is_number_unsigned())
	{
		const auto tmp = static_cast<DWRITE_FONT_STRETCH>(stretch.get<uint32_t>());
		RETURN_IF_FAILED(text_layout->SetFontStretch(tmp, range));
	}

	auto& strikethrough = j["Strikethrough"];
	if (strikethrough.is_boolean())
	{
		const BOOL tmp = strikethrough.get<bool>() ? TRUE : FALSE;
		RETURN_IF_FAILED(text_layout->SetStrikethrough(tmp, range));
	}

	auto& underline = j["Underline"];
	if (underline.is_boolean())
	{
		const BOOL tmp = underline.get<bool>() ? TRUE : FALSE;
		RETURN_IF_FAILED(text_layout->SetUnderline(tmp, range));
	}
	return S_OK;
}

HRESULT apply_fonts(IDWriteTextLayout* text_layout, wil::zwstring_view font_string)
{
	auto fonts = JSONHelper::parse(font_string);
	RETURN_HR_IF(E_INVALIDARG, !fonts.is_array());

	for (auto&& font : fonts)
	{
		DWRITE_TEXT_RANGE range{};
		RETURN_IF_FAILED(JSONHelper::to_dwrite_text_range(font, range));
		RETURN_IF_FAILED(apply_font(text_layout, font, range));
	}
	return S_OK;
}

HRESULT apply_trimming(IDWriteTextFormat* text_format, uint32_t trimming_granularity)
{
	DWRITE_TRIMMING trimming{};
	trimming.granularity = static_cast<DWRITE_TRIMMING_GRANULARITY>(trimming_granularity);

	wil::com_ptr_t<IDWriteInlineObject> trimmingSign;
	RETURN_IF_FAILED(g_dwrite_factory->CreateEllipsisTrimmingSign(text_format, &trimmingSign));
	RETURN_IF_FAILED(text_format->SetTrimming(&trimming, trimmingSign.get()));
	return S_OK;
}
