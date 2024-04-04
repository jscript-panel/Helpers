#pragma once

using JSON = nlohmann::json;
using VariantArgs = std::vector<_variant_t>;

using StringMap = std::map<std::string, std::string>;
using StringPair = std::pair<std::string, std::string>;
using Strings = std::vector<std::string>;
using WStrings = std::vector<std::wstring>;
using pfc::string8;

inline pfc::string_base& operator<<(pfc::string_base& fmt, const std::string& source)
{
	fmt.add_string_(source.c_str());
	return fmt;
}

static constexpr wil::zstring_view CRLF = "\r\n";

namespace js
{
	template <typename T>
	concept IsNum = std::integral<T> || std::floating_point<T>;

	template <typename T>
	static pfc::array_t<T> pfc_array(size_t count)
	{
		pfc::array_t<T> arr;
		arr.set_size(count);
		return arr;
	}

	static void** arg_helper(auto arg)
	{
		return reinterpret_cast<void**>(arg);
	}

	static COLORREF to_colorref(int colour)
	{
		return RGB(colour >> RED_SHIFT, colour >> GREEN_SHIFT, colour >> BLUE_SHIFT);
	}

	static D2D1_RECT_F to_rectf(float x = 0.f, float y = 0.f, float w = 0.f, float h = 0.f)
	{
		return D2D1::RectF(x, y, x + w, y + h);
	}

	static HWND to_wnd(uintptr_t window_id)
	{
		return reinterpret_cast<HWND>(window_id);
	}

	static Strings split_string(std::string_view text, std::string_view delims)
	{
		return text | std::views::split(delims) | std::ranges::to<Strings>();
	}

	static VARIANT_BOOL to_variant_bool(auto b)
	{
		return b ? VARIANT_TRUE : VARIANT_FALSE;
	}

	static bool compare_string(wil::zstring_view a, wil::zstring_view b)
	{
		return stricmp_utf8(a.data(), b.data()) == 0;
	}

	static bool compare_string(wil::zwstring_view a, wil::zwstring_view b)
	{
		return _wcsicmp(a.data(), b.data()) == 0;
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

	static std::string from_wide(std::wstring_view str)
	{
		return pfc::utf8FromWide(str.data(), str.length()).get_ptr();
	}

	static std::string get_resource_text(int id)
	{
		const auto res = uLoadResource(core_api::get_my_instance(), uMAKEINTRESOURCE(id), "TEXT");
		return std::string(static_cast<const char*>(res->GetPointer()), res->GetSize());
	}

	static std::string print_guid(const GUID& g)
	{
		return fmt::format("{{{}}}", pfc::print_guid(g).get_ptr());
	}

	static std::wstring to_wide(std::string_view str)
	{
		return pfc::wideFromUTF8(str.data(), str.length()).c_str();
	}

	static std::wstring wdisplay_path(wil::zstring_view path)
	{
		string8 tmp;
		filesystem::g_get_display_path(path.data(), tmp);
		return to_wide(tmp);
	}

	static uint32_t to_uint(IsNum auto num)
	{
		return static_cast<uint32_t>(num);
	}

	static void sort_strings(WStrings& strings)
	{
		std::ranges::sort(strings, [](auto&& lhs, auto&& rhs)
			{
				return StrCmpLogicalW(lhs.data(), rhs.data()) < 0;
			});
	}

	static BSTR to_bstr(std::string_view str)
	{
		const auto wstr = to_wide(str);
		return SysAllocString(wstr.data());
	}

	static D2D1_COLOR_F to_colorf(int64_t colour)
	{
		const auto a = to_float((colour >> 24) & 0xff) / 255.f;
		return D2D1::ColorF(to_uint(colour), a);
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

	static int to_argb(COLORREF colour)
	{
		const auto c = GetRValue(colour) << RED_SHIFT | GetGValue(colour) << GREEN_SHIFT | GetBValue(colour) << BLUE_SHIFT | 0xff000000;
		return to_int(c);
	}

	static JSON json_parse(wil::zstring_view str)
	{
		return JSON::parse(str, nullptr, false);
	}

	static JSON json_parse(wil::zwstring_view str)
	{
		const auto ustr = from_wide(str);
		return json_parse(ustr);
	}

	static std::string json_to_string(JSON& j)
	{
		if (j.is_string())
		{
			return j.get<std::string>();
		}
		else if (j.is_number())
		{
			return j.dump();
		}

		return std::string();
	}

	static std::wstring json_to_wstring(JSON& j)
	{
		const auto str = json_to_string(j);
		return to_wide(str);
	}

	static Strings json_to_strings(JSON j)
	{
		if (!j.is_array()) j = JSON::array({ j });

		auto transform = [](auto&& j2) { return json_to_string(j2); };
		auto filter = [](auto&& str) { return str.length() > 0; };
		return j | std::views::transform(transform) | std::views::filter(filter) | std::ranges::to<Strings>();
	}
}
