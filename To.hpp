#pragma once

using JSON = nlohmann::json;

namespace js
{
	template <typename T>
	concept IsNum = std::integral<T> || std::floating_point<T>;

	template <typename T>
	concept IsEnum = std::is_enum_v<T>;

	template <typename T>
	static T to_enum(JSON& obj) requires IsEnum<T>
	{
		return static_cast<T>(obj.get<uint32_t>());
	}

	template <typename T>
	static T to_enum(std::wstring_view str) requires IsEnum<T>
	{
		return static_cast<T>(std::stoul(str.data()));
	}

	static BOOL to_BOOL(JSON& obj)
	{
		return obj.get<bool>() ? TRUE : FALSE;
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

	static float to_float(IsNum auto num)
	{
		return static_cast<float>(num);
	}

	static int to_int(std::integral auto num)
	{
		return static_cast<int>(num);
	}

	static intptr_t to_int_ptr(size_t num)
	{
		return static_cast<intptr_t>(num);
	}

	static std::optional<int> to_int(std::string_view str)
	{
		if (pfc::string_is_numeric(str.data()))
			return std::stoi(str.data());

		return std::nullopt;
	}

	static uint32_t to_uint(IsNum auto num)
	{
		return static_cast<uint32_t>(num);
	}

	static D2D1_RECT_F to_rectf(float x, float y, float w, float h)
	{
		return D2D1::RectF(x, y, x + w, y + h);
	}

	static D2D1_RECT_F to_rectf(const D2D1_SIZE_U& size)
	{
		return D2D1::RectF(
			0.f,
			0.f,
			to_float(size.width),
			to_float(size.height)
		);
	}

	static WICRect to_WICRect(const D2D1_SIZE_U& size)
	{
		WICRect rect{};
		rect.X = 0;
		rect.Y = 0;
		rect.Width = to_int(size.width);
		rect.Height = to_int(size.height);
		return rect;
	}
}
