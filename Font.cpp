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
	if (!obj.is_object())
		return;

	auto name = js::json_to_wstring(obj["Name"]);
	auto& size = obj["Size"];
	auto& weight = obj["Weight"];
	auto& style = obj["Style"];
	auto& stretch = obj["Stretch"];
	auto& strikethrough = obj["Strikethrough"];
	auto& underline = obj["Underline"];

	if (name.length())
	{
		m_name = name;
	}

	if (size.is_number())
	{
		m_size = size.get<float>();
	}

	if (weight.is_number_unsigned())
	{
		m_weight = js::to_enum<DWRITE_FONT_WEIGHT>(weight);
	}

	if (style.is_number_unsigned())
	{
		m_style = js::to_enum<DWRITE_FONT_STYLE>(style);
	}

	if (stretch.is_number_unsigned())
	{
		m_stretch = js::to_enum<DWRITE_FONT_STRETCH>(stretch);
	}

	if (strikethrough.is_boolean())
	{
		m_strikethrough = js::to_BOOL(strikethrough);
	}

	if (underline.is_boolean())
	{
		m_underline = js::to_BOOL(underline);
	}
}

Font::Font(std::wstring_view str)
{
	const auto parts = js::split_string(str, TAB);

	if (parts.size() != 6)
		return;

	m_name = parts[0];
	m_size = std::stof(parts[1]);
	m_weight = js::to_enum<DWRITE_FONT_WEIGHT>(parts[2]);
	m_style = js::to_enum<DWRITE_FONT_STYLE>(parts[3]);

	if (parts[4] == L"1")
	{
		m_underline = TRUE;
	}

	if (parts[5] == L"1")
	{
		m_strikethrough = TRUE;
	}
}

Font::Font(const LOGFONT& lf)
{
	const auto hr = [&]
		{
			wil::com_ptr_t<IDWriteFont> font;
			wil::com_ptr_t<IDWriteFontFamily> font_family;
			wil::com_ptr_t<IDWriteLocalizedStrings> family_names;

			RETURN_IF_FAILED(factory::gdi_interop->CreateFontFromLOGFONT(&lf, &font));
			RETURN_IF_FAILED(font->GetFontFamily(&font_family));
			RETURN_IF_FAILED(font_family->GetFamilyNames(&family_names));

			m_name = factory::get_font_name(family_names.get());
			m_weight = font->GetWeight();
			m_style = font->GetStyle();
			m_stretch = font->GetStretch();
			return S_OK;
		}();

	if FAILED(hr)
	{
		m_weight = static_cast<DWRITE_FONT_WEIGHT>(lf.lfWeight);

		if (lf.lfItalic != 0)
		{
			m_style = DWRITE_FONT_STYLE_ITALIC;
		}
	}

	if (lf.lfHeight == 0)
	{
		m_size = 12.f;
	}
	else
	{
		m_size = js::to_float(std::abs(lf.lfHeight));
	}
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

std::string Font::to_string() const
{
	auto j = JSON::object();
	j["Name"] = js::from_wide(m_name);
	j["Size"] = m_size;
	j["Weight"] = std::to_underlying(m_weight);
	j["Style"] = std::to_underlying(m_style);
	j["Stretch"] = std::to_underlying(m_stretch);
	return j.dump(4);
}
