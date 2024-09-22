#pragma once
#include "FormatParams.hpp"

struct Font
{
	Font() = default;
	Font(std::wstring_view name, float size, uint32_t weight, uint32_t style, uint32_t stretch);
	Font(JSON& obj);
	Font(std::wstring_view str);
	Font(const LOGFONT& lf);

	HRESULT create_format(wil::com_ptr_t<IDWriteTextFormat>& text_format, const FormatParams& params) const;
	std::string to_string() const;

	BOOL m_strikethrough{}, m_underline{};
	DWRITE_FONT_WEIGHT m_weight = DWRITE_FONT_WEIGHT_NORMAL;
	DWRITE_FONT_STYLE m_style = DWRITE_FONT_STYLE_NORMAL;
	DWRITE_FONT_STRETCH m_stretch = DWRITE_FONT_STRETCH_NORMAL;
	float m_size = 16.f;
	std::wstring m_name = factory::DefaultFont.data();
};
