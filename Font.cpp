#include "stdafx.hpp"
#include "Font.hpp"

Font::Font(std::wstring_view name, float size, uint32_t weight, uint32_t style, uint32_t stretch)
	: m_name(name)
	, m_size(size)
	, m_weight(static_cast<DWRITE_FONT_WEIGHT>(weight))
	, m_style(static_cast<DWRITE_FONT_STYLE>(style))
	, m_stretch(static_cast<DWRITE_FONT_STRETCH>(stretch)) {}

Font::Font(JSON& obj)
{
	if (!obj.is_object()) return;

	auto name = js::json_to_wstring(obj["Name"]);
	if (name.length())
	{
		m_name = name;
	}

	auto& size = obj["Size"];
	if (size.is_number())
	{
		m_size = get<float>(size);
	}

	auto& weight = obj["Weight"];
	if (weight.is_number_unsigned())
	{
		m_weight = get<DWRITE_FONT_WEIGHT>(weight);
	}

	auto& style = obj["Style"];
	if (style.is_number_unsigned())
	{
		m_style = get<DWRITE_FONT_STYLE>(style);
	}

	auto& stretch = obj["Stretch"];
	if (stretch.is_number_unsigned())
	{
		m_stretch = get<DWRITE_FONT_STRETCH>(stretch);
	}

	auto& strikethrough = obj["Strikethrough"];
	if (strikethrough.is_boolean())
	{
		m_strikethrough = get_BOOL(strikethrough);
	}

	auto& underline = obj["Underline"];
	if (underline.is_boolean())
	{
		m_underline = get_BOOL(underline);
	}
}

Font::Font(std::wstring_view str)
{
	const auto parts = js::split_string(str, TAB);
	if (parts.size() != 6) return;

	m_name = parts[0];
	m_size = get<float>(parts[1]);
	m_weight = get<DWRITE_FONT_WEIGHT>(parts[2]);
	m_style = get<DWRITE_FONT_STYLE>(parts[3]);

	if (parts[4] == L"1") m_underline = TRUE;
	if (parts[5] == L"1") m_strikethrough = TRUE;
}

HRESULT Font::create_format(wil::com_ptr_t<IDWriteTextFormat>& text_format, const FormatParams& params) const
{
	wil::com_ptr_t<IDWriteInlineObject> trimmingSign;

	RETURN_IF_FAILED(factory::dwrite->CreateTextFormat(m_name.data(), nullptr, m_weight, m_style, m_stretch, m_size, L"", &text_format));
	RETURN_IF_FAILED(text_format->SetTextAlignment(params.m_text_alignment));
	RETURN_IF_FAILED(text_format->SetParagraphAlignment(params.m_paragraph_alignment));
	RETURN_IF_FAILED(text_format->SetWordWrapping(params.m_word_wrapping));
	RETURN_IF_FAILED(factory::dwrite->CreateEllipsisTrimmingSign(text_format.get(), &trimmingSign));
	RETURN_IF_FAILED(text_format->SetTrimming(&params.m_trimming, trimmingSign.get()));
	return S_OK;
}
