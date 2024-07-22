#pragma once

using JSON = nlohmann::json;
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

static constexpr std::wstring_view TAB = L"\t";
static constexpr std::wstring_view BEL = L"\7";
static constexpr std::wstring_view ETX = L"\3";
static constexpr std::string_view CRLF = "\r\n";

namespace js
{
	// based on SDK/titleformat/remove_color_marks
	static std::wstring remove_mark(const wchar_t* src, wchar_t ch)
	{
		if (!wcschr(src, ch))
		{
			return src;
		}

		std::wstring ret;

		while (*src)
		{
			if (*src == ch)
			{
				src++;
				while (*src && *src != ch)
				{
					src++;
				}

				if (*src == ch)
				{
					src++;
				}
			}
			else
			{
				ret += *src++;
			}
		}

		return ret;
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

	static std::string from_wide(std::wstring_view str)
	{
		return pfc::utf8FromWide(str.data(), str.length()).get_ptr();
	}

	static std::wstring to_wide(std::string_view str)
	{
		return pfc::wideFromUTF8(str.data(), str.length()).c_str();
	}

	static BSTR to_bstr(std::string_view str)
	{
		const auto wstr = to_wide(str);
		return SysAllocString(wstr.data());
	}

	static JSON json_parse(std::string_view str)
	{
		return JSON::parse(str, nullptr, false);
	}

	static JSON json_parse(std::wstring_view str)
	{
		const auto ustr = from_wide(str);
		return json_parse(ustr);
	}

	static Strings json_to_strings(JSON j)
	{
		if (!j.is_array()) j = JSON::array({ j });

		auto transform = [](auto&& j2) { return json_to_string(j2); };
		auto filter = [](auto&& str) { return str.length() > 0; };
		return j | std::views::transform(transform) | std::views::filter(filter) | std::ranges::to<Strings>();
	}

	static Strings split_string(std::string_view text, std::string_view delims)
	{
		return text | std::views::split(delims) | std::ranges::to<Strings>();
	}

	static WStrings split_string(std::wstring_view text, std::wstring_view delims)
	{
		return text | std::views::split(delims) | std::ranges::to<WStrings>();
	}

	static bool compare_string(std::string_view a, std::string_view b)
	{
		return stricmp_utf8(a.data(), b.data()) == 0;
	}

	static bool compare_string(std::wstring_view a, std::wstring_view b)
	{
		return _wcsicmp(a.data(), b.data()) == 0;
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

	static std::wstring json_to_wstring(JSON& j)
	{
		const auto str = json_to_string(j);
		return to_wide(str);
	}

	static std::wstring remove_marks(std::wstring_view str)
	{
		std::wstring ret(str);
		ret = remove_mark(ret.data(), 3);
		ret = remove_mark(ret.data(), 7);
		return ret;
	}

	static void sort_strings(WStrings& strings)
	{
		std::ranges::sort(strings, [](auto&& lhs, auto&& rhs)
			{
				return StrCmpLogicalW(lhs.data(), rhs.data()) < 0;
			});
	}
}
