#include "stdafx.hpp"
#include "WriteTextLayout.hpp"

HRESULT WriteTextLayout::to_range(JSON& obj, DWRITE_TEXT_RANGE& range, bool verify_colour)
{
	if (obj.is_object() && obj["Start"].is_number_unsigned() && obj["Length"].is_number_unsigned())
	{
		RETURN_HR_IF(E_INVALIDARG, verify_colour && !obj["Colour"].is_number());

		range.startPosition = obj["Start"].get<uint32_t>();
		range.length = obj["Length"].get<uint32_t>();
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT WriteTextLayout::apply_colour(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, const D2D1_COLOR_F& colour, const DWRITE_TEXT_RANGE& range)
{
	wil::com_ptr_t<ID2D1SolidColorBrush> brush;
	RETURN_IF_FAILED(context->CreateSolidColorBrush(colour, &brush));
	RETURN_IF_FAILED(text_layout->SetDrawingEffect(brush.get(), range));
	return S_OK;
}

HRESULT WriteTextLayout::apply_colours(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, JSON& colours)
{
	RETURN_HR_IF(E_INVALIDARG, !colours.is_array());

	DWRITE_TEXT_RANGE range{};

	for (auto&& colour : colours)
	{
		RETURN_IF_FAILED(to_range(colour, range, true));

		const auto colourf = js::to_colorf(colour["Colour"]);
		RETURN_IF_FAILED(apply_colour(text_layout, context, colourf, range));
	}

	return S_OK;
}

HRESULT WriteTextLayout::apply_colours(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, std::wstring_view text)
{
	uint32_t start = js::to_uint(text.find(ETX));
	const auto parts = js::split_string(text, ETX);

	for (size_t i = 1; i < parts.size(); i += 2)
	{
		const auto& colour_part = parts[i];
		const auto& text_part = parts[i + 1];
		if (text_part.empty()) continue;

		const auto range = DWRITE_TEXT_RANGE(start, js::lengthu(text_part));
		start += range.length;

		if (!colour_part.empty())
		{
			RETURN_IF_FAILED(apply_colour(text_layout, context, js::to_colorf(colour_part), range));
		}
	}

	return S_OK;
}

HRESULT WriteTextLayout::apply_font(IDWriteTextLayout* text_layout, const Font& font, const DWRITE_TEXT_RANGE& range)
{
	RETURN_IF_FAILED(text_layout->SetFontFamilyName(font.m_name.data(), range));
	RETURN_IF_FAILED(text_layout->SetFontSize(font.m_size, range));
	RETURN_IF_FAILED(text_layout->SetFontWeight(font.m_weight, range));
	RETURN_IF_FAILED(text_layout->SetFontStyle(font.m_style, range));
	RETURN_IF_FAILED(text_layout->SetFontStretch(font.m_stretch, range));
	RETURN_IF_FAILED(text_layout->SetStrikethrough(font.m_strikethrough, range));
	RETURN_IF_FAILED(text_layout->SetUnderline(font.m_underline, range));
	return S_OK;
}

HRESULT WriteTextLayout::apply_fonts(IDWriteTextLayout* text_layout, JSON& jfonts)
{
	for (auto&& jfont : jfonts)
	{
		DWRITE_TEXT_RANGE range{};
		const auto font = Font(jfont);
		RETURN_IF_FAILED(to_range(jfont, range));
		RETURN_IF_FAILED(apply_font(text_layout, font, range));
	}

	return S_OK;
}

HRESULT WriteTextLayout::apply_fonts(IDWriteTextLayout* text_layout, std::wstring_view text)
{
	uint32_t start = js::to_uint(text.find(BEL));
	const auto parts = js::split_string(text, BEL);

	for (size_t i = 1; i < parts.size(); i += 2)
	{
		const auto& font_part = parts[i];
		const auto& text_part = parts[i + 1];
		if (text_part.empty()) continue;

		const auto range = DWRITE_TEXT_RANGE(start, js::lengthu(text_part));
		start += range.length;

		if (!font_part.empty())
		{
			const auto font = Font(font_part);
			RETURN_IF_FAILED(apply_font(text_layout, font, range));
		}
	}

	return S_OK;
}

HRESULT WriteTextLayout::apply_line_spacing(IDWriteTextLayout3* text_layout)
{
	DWRITE_LINE_SPACING spacing{};
	spacing.method = DWRITE_LINE_SPACING_METHOD_PROPORTIONAL;
	spacing.height = 1.f;
	spacing.baseline = 1.f;
	spacing.leadingBefore = 0.f;
	spacing.fontLineGapUsage = DWRITE_FONT_LINE_GAP_USAGE_DISABLED;

	return text_layout->SetLineSpacing(&spacing);
}

HRESULT WriteTextLayout::create(wil::com_ptr_t<IDWriteTextLayout>& text_layout, const Font& font, const FormatParams& params, std::wstring_view text, float width, float height)
{
	const auto clean = js::remove_marks(text);
	const auto length = js::lengthu(clean);
	const auto range = DWRITE_TEXT_RANGE(0U, length);
	wil::com_ptr_t<IDWriteTextFormat> text_format;

	RETURN_IF_FAILED(font.create_format(text_format, params));
	RETURN_IF_FAILED(factory::dwrite->CreateTextLayout(clean.data(), length, text_format.get(), width, height, &text_layout));
	RETURN_IF_FAILED(text_layout->SetStrikethrough(font.m_strikethrough, range));
	RETURN_IF_FAILED(text_layout->SetUnderline(font.m_underline, range));
	RETURN_IF_FAILED(text_layout->SetTypography(factory::typography.get(), range));
	return S_OK;
}

HRESULT WriteTextLayout::create3(wil::com_ptr_t<IDWriteTextLayout3>& text_layout, const Font& font, const FormatParams& params, std::wstring_view text)
{
	wil::com_ptr_t<IDWriteTextLayout> temp_layout;

	RETURN_IF_FAILED(create(temp_layout, font, params, text, FLT_MAX, FLT_MAX));
	RETURN_HR_IF(E_FAIL, !temp_layout.try_query_to(IID_PPV_ARGS(&text_layout)));
	return S_OK;
}
