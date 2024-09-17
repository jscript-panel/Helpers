#include "stdafx.hpp"
#include "WriteTextLayout.hpp"

WriteTextLayout::ColourRanges WriteTextLayout::parse_tf_colours(std::wstring_view text)
{
	ColourRanges colour_ranges;
	const auto clean = js::remove_mark(text, BEL);
	auto start = js::to_uint(clean.find(ETX));

	if (start != std::wstring_view::npos)
	{
		const auto view = js::split_string(clean, ETX) | std::views::drop(1) | std::views::chunk(2);

		for (auto&& parts : view)
		{
			const auto& colour_part = parts[0];
			const auto& text_part = parts[1];
			if (text_part.empty())
				continue;

			const auto range = DWRITE_TEXT_RANGE(start, js::lengthu(text_part));
			start += range.length;

			if (!colour_part.empty())
			{
				ColourRange colour_range{};
				colour_range.colour = js::to_colorf(colour_part);
				colour_range.range = range;
				colour_ranges.emplace_back(colour_range);
			}
		}
	}

	return colour_ranges;
}

WriteTextLayout::FontRanges WriteTextLayout::parse_tf_fonts(std::wstring_view text)
{
	FontRanges font_ranges;
	const auto clean = js::remove_mark(text, ETX);
	auto start = js::to_uint(clean.find(BEL));

	if (start != std::wstring_view::npos)
	{
		const auto view = js::split_string(clean, BEL) | std::views::drop(1) | std::views::chunk(2);

		for (auto&& parts : view)
		{
			const auto& font_part = parts[0];
			const auto& text_part = parts[1];
			if (text_part.empty())
				continue;

			const auto range = DWRITE_TEXT_RANGE(start, js::lengthu(text_part));
			start += range.length;

			if (!font_part.empty())
			{
				FontRange font_range{};
				font_range.font = Font(font_part);
				font_range.range = range;
				font_ranges.emplace_back(font_range);
			}
		}
	}

	return font_ranges;
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
	RETURN_IF_FAILED(set_line_spacing(text_layout.get()));
	return S_OK;
}

HRESULT WriteTextLayout::set_colours(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, const ColourRanges& colour_ranges)
{
	for (auto&& [colour, range] : colour_ranges)
	{
		wil::com_ptr_t<ID2D1SolidColorBrush> brush;
		RETURN_IF_FAILED(context->CreateSolidColorBrush(colour, &brush));
		RETURN_IF_FAILED(text_layout->SetDrawingEffect(brush.get(), range));
	}

	return S_OK;
}

HRESULT WriteTextLayout::set_colours_json(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, JSON& jcolours)
{
	RETURN_HR_IF(E_INVALIDARG, !jcolours.is_array());

	ColourRanges colour_ranges(jcolours.size());

	for (auto&& [colour_range, jcolour] : std::views::zip(colour_ranges, jcolours))
	{
		RETURN_IF_FAILED(to_range(jcolour, colour_range.range, true));
		colour_range.colour = js::to_colorf(jcolour["Colour"]);
	}

	return set_colours(text_layout, context, colour_ranges);
}

HRESULT WriteTextLayout::set_fonts(IDWriteTextLayout* text_layout, const FontRanges& font_ranges)
{
	for (auto&& [font, range] : font_ranges)
	{
		RETURN_IF_FAILED(text_layout->SetFontFamilyName(font.m_name.data(), range));
		RETURN_IF_FAILED(text_layout->SetFontSize(font.m_size, range));
		RETURN_IF_FAILED(text_layout->SetFontWeight(font.m_weight, range));
		RETURN_IF_FAILED(text_layout->SetFontStyle(font.m_style, range));
		RETURN_IF_FAILED(text_layout->SetFontStretch(font.m_stretch, range));
		RETURN_IF_FAILED(text_layout->SetStrikethrough(font.m_strikethrough, range));
		RETURN_IF_FAILED(text_layout->SetUnderline(font.m_underline, range));
	}

	return S_OK;
}

HRESULT WriteTextLayout::set_fonts_json(IDWriteTextLayout* text_layout, JSON& jfonts)
{
	if (!jfonts.is_array())
		return S_OK;

	FontRanges font_ranges(jfonts.size());

	for (auto&& [font_range, jfont] : std::views::zip(font_ranges, jfonts))
	{
		RETURN_IF_FAILED(to_range(jfont, font_range.range));
		font_range.font = Font(jfont);
	}

	return set_fonts(text_layout, font_ranges);
}

HRESULT WriteTextLayout::set_line_spacing(IDWriteTextLayout3* text_layout)
{
	DWRITE_LINE_SPACING spacing{};
	spacing.method = DWRITE_LINE_SPACING_METHOD_PROPORTIONAL;
	spacing.height = 1.f;
	spacing.baseline = 1.f;
	spacing.leadingBefore = 0.f;
	spacing.fontLineGapUsage = DWRITE_FONT_LINE_GAP_USAGE_DISABLED;

	return text_layout->SetLineSpacing(&spacing);
}

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

float WriteTextLayout::calc_text_width(std::wstring_view text, const Font& font)
{
	const auto fonts = parse_tf_fonts(text);
	wil::com_ptr_t<IDWriteTextLayout> text_layout;

	if FAILED(create(text_layout, font, FormatParams(), text, FLT_MAX, FLT_MAX))
		return 0.f;

	if FAILED(set_fonts(text_layout.get(), fonts))
		return 0.f;

	DWRITE_TEXT_METRICS metrics{};
	text_layout->GetMetrics(&metrics);
	return metrics.widthIncludingTrailingWhitespace;
}
