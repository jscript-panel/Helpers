#pragma once
#include "FormatParams.hpp"

struct Font
{
	Font() = default;
	Font(std::wstring_view name, float size, uint32_t weight, uint32_t style, uint32_t stretch);
	Font(JSON& obj);
	Font(std::wstring_view str);

	static BOOL get_BOOL(JSON& obj)
	{
		return obj.get<bool>() ? TRUE : FALSE;
	}

	template <typename T>
	static T get(JSON& obj)
	{
		return static_cast<T>(obj.get<uint32_t>());
	}

	template <typename T>
	static T get(std::wstring_view str)
	{
		return static_cast<T>(std::stoul(str.data()));
	}

	HRESULT create_format(wil::com_ptr_t<IDWriteTextFormat>& text_format, const FormatParams& params) const;

	BOOL m_strikethrough{}, m_underline{};
	DWRITE_FONT_WEIGHT m_weight = DWRITE_FONT_WEIGHT_NORMAL;
	DWRITE_FONT_STYLE m_style = DWRITE_FONT_STYLE_NORMAL;
	DWRITE_FONT_STRETCH m_stretch = DWRITE_FONT_STRETCH_NORMAL;
	float m_size = 16.f;
	std::wstring m_name = factory::DefaultFont.data();
};
