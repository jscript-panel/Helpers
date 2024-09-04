#pragma once

namespace js
{
	static int hex_digit_to_int(int ch)
	{
		if (ch >= '0' && ch <= '9')
		{
			return ch - '0';
		}
		else if (ch >= 'A' && ch <= 'F')
		{
			return ch - 'A' + 10;
		}
		else if (ch >= 'a' && ch <= 'f')
		{
			return ch - 'a' + 10;
		}
		return 0;
	}

	static int to_argb(COLORREF colour)
	{
		const auto c = GetRValue(colour) << RED_SHIFT | GetGValue(colour) << GREEN_SHIFT | GetBValue(colour) << BLUE_SHIFT | 0xff000000;
		return to_int(c);
	}

	static COLORREF to_colorref(int colour)
	{
		return RGB(colour >> RED_SHIFT, colour >> GREEN_SHIFT, colour >> BLUE_SHIFT);
	}

	static D2D1_COLOR_F to_colorf(int64_t colour)
	{
		const auto a = to_float((colour >> 24) & 0xff) / 255.f;
		return D2D1::ColorF(to_uint(colour), a);
	}

	static D2D1_COLOR_F to_colorf(JSON& colour)
	{
		return to_colorf(colour.get<int64_t>());
	}

	static D2D1_COLOR_F to_colorf(VARIANT colour)
	{
		if FAILED(VariantChangeType(&colour, &colour, 0, VT_I8))
			return factory::White;

		return to_colorf(colour.llVal);
	}

	static D2D1_COLOR_F to_colorf(std::wstring_view hex)
	{
		const int r = hex_digit_to_int(hex.at(0)) << 4 | hex_digit_to_int(hex.at(1));
		const int g = hex_digit_to_int(hex.at(2)) << 4 | hex_digit_to_int(hex.at(3));
		const int b = hex_digit_to_int(hex.at(4)) << 4 | hex_digit_to_int(hex.at(5));
		return D2D1::ColorF(RGB(r, g, b));
	}
}
