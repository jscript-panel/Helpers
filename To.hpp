#pragma once

template <typename T>
concept IsNum = std::integral<T> || std::floating_point<T>;

static COLORREF to_colorref(int colour)
{
	return RGB(colour >> RED_SHIFT, colour >> GREEN_SHIFT, colour >> BLUE_SHIFT);
}

static D2D1_RECT_F to_rectf(float x, float y, float w, float h)
{
	return D2D1::RectF(x, y, x + w, y + h);
}

static HWND to_wnd(uintptr_t window_id)
{
	return reinterpret_cast<HWND>(window_id);
}

static VARIANT_BOOL to_variant_bool(auto b)
{
	return b ? VARIANT_TRUE : VARIANT_FALSE;
}

static bool to_bool(VARIANT_BOOL vb)
{
	return vb != VARIANT_FALSE;
}

static int to_int(std::integral auto num)
{
	return static_cast<int>(num);
}

// when fb2k SDK methods return SIZE_MAX on failure, we want -1 for JavaScript
static intptr_t to_int_ptr(size_t num)
{
	return static_cast<intptr_t>(num);
}

static uint32_t to_uint(IsNum auto num)
{
	return static_cast<uint32_t>(num);
}

static D2D1_COLOR_F to_colorf(int64_t colour)
{
	const auto a = static_cast<float>((colour >> 24) & 0xff);
	return D2D1::ColorF(to_uint(colour), 1.f / 255.f * a);
}

static int to_argb(COLORREF colour)
{
	const auto c = GetRValue(colour) << RED_SHIFT | GetGValue(colour) << GREEN_SHIFT | GetBValue(colour) << BLUE_SHIFT | 0xff000000;
	return to_int(c);
}
