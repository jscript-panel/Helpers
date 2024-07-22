#include "stdafx.hpp"
#include "WriteText.hpp"

DWRITE_TEXT_RANGE WriteText::to_range(size_t start, size_t length)
{
	DWRITE_TEXT_RANGE range{};
	range.startPosition = js::to_uint(start);
	range.length = js::to_uint(length);
	return range;
}

HRESULT WriteText::to_range(JSON& obj, DWRITE_TEXT_RANGE& range, bool verify_colour)
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

HRESULT WriteText::apply_colour(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, const D2D1_COLOR_F& colour, const DWRITE_TEXT_RANGE& range)
{
	wil::com_ptr_t<ID2D1SolidColorBrush> brush;
	RETURN_IF_FAILED(context->CreateSolidColorBrush(colour, &brush));
	RETURN_IF_FAILED(text_layout->SetDrawingEffect(brush.get(), range));
	return S_OK;
}

HRESULT WriteText::apply_colours(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, JSON& colours)
{
	DWRITE_TEXT_RANGE range{};

	for (auto&& colour : colours)
	{
		RETURN_IF_FAILED(to_range(colour, range, true));

		const auto colourf = js::to_colorf(colour["Colour"]);
		RETURN_IF_FAILED(apply_colour(text_layout, context, colourf, range));
	}

	return S_OK;
}

HRESULT WriteText::apply_colours(IDWriteTextLayout* text_layout, ID2D1DeviceContext* context, std::wstring_view text)
{
	size_t start = text.find(ETX);
	const auto parts = js::split_string(text, ETX);

	for (size_t i = 1; i < parts.size(); i += 2)
	{
		const auto& colour_part = parts[i];
		const auto& text_part = parts[i + 1];
		if (text_part.empty()) continue;

		const auto range = to_range(start, text_part.length());
		start += range.length;

		if (!colour_part.empty())
		{
			RETURN_IF_FAILED(apply_colour(text_layout, context, js::to_colorf(colour_part), range));
		}
	}

	return S_OK;
}

HRESULT WriteText::apply_font(IDWriteTextLayout* text_layout, const Font& font, const DWRITE_TEXT_RANGE& range)
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

HRESULT WriteText::apply_fonts(IDWriteTextLayout* text_layout, JSON& jfonts)
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

HRESULT WriteText::apply_fonts(IDWriteTextLayout* text_layout, std::wstring_view text)
{
	size_t start = text.find(BEL);
	const auto parts = js::split_string(text, BEL);

	for (size_t i = 1; i < parts.size(); i += 2)
	{
		const auto& font_part = parts[i];
		const auto& text_part = parts[i + 1];
		if (text_part.empty()) continue;

		const auto range = to_range(start, text_part.length());
		start += range.length;

		if (!font_part.empty())
		{
			const auto font = Font(font_part);
			RETURN_IF_FAILED(apply_font(text_layout, font, range));
		}
	}

	return S_OK;
}

HRESULT WriteText::create_format(wil::com_ptr_t<IDWriteTextFormat>& text_format, const Font& font, const FormatParams& params)
{
	wil::com_ptr_t<IDWriteInlineObject> trimmingSign;

	RETURN_IF_FAILED(factory::dwrite->CreateTextFormat(font.m_name.data(), nullptr, font.m_weight, font.m_style, font.m_stretch, font.m_size, L"", &text_format));
	RETURN_IF_FAILED(text_format->SetTextAlignment(params.m_text_alignment));
	RETURN_IF_FAILED(text_format->SetParagraphAlignment(params.m_paragraph_alignment));
	RETURN_IF_FAILED(text_format->SetWordWrapping(params.m_word_wrapping));
	RETURN_IF_FAILED(factory::dwrite->CreateEllipsisTrimmingSign(text_format.get(), &trimmingSign));
	RETURN_IF_FAILED(text_format->SetTrimming(&params.m_trimming, trimmingSign.get()));
	return S_OK;
}

HRESULT WriteText::create_layout(wil::com_ptr_t<IDWriteTextLayout>& text_layout, IDWriteTextFormat* text_format, std::wstring_view text, float width, float height)
{
	uint32_t length{};

	if (text.contains(BEL) || text.contains(ETX))
	{
		auto clean = js::remove_marks(text);
		length = js::to_uint(clean.length());
		RETURN_IF_FAILED(factory::dwrite->CreateTextLayout(clean.data(), length, text_format, width, height, &text_layout));
	}
	else
	{
		length = js::to_uint(text.length());
		RETURN_IF_FAILED(factory::dwrite->CreateTextLayout(text.data(), length, text_format, width, height, &text_layout));
	}

	return text_layout->SetTypography(factory::typography.get(), { 0U, length });
}
