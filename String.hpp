#pragma once

using StringMap = std::map<std::string, std::string>;
using StringPair = std::pair<std::string, std::string>;
using Strings = std::vector<std::string>;
using pfc::string8;

inline pfc::string_base& operator<<(pfc::string_base& fmt, const std::string& source)
{
	fmt.add_string_(source.c_str());
	return fmt;
}

static constexpr std::string_view CRLF = "\r\n";

namespace js
{
	static Strings split_string(std::string_view text, std::string_view delims)
	{
		return text | std::views::split(delims) | std::ranges::to<Strings>();
	}

	static bool compare_string(std::string_view a, std::string_view b)
	{
		return stricmp_utf8(a.data(), b.data()) == 0;
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

	static std::string get_resource_text(int id)
	{
		const auto res = uLoadResource(core_api::get_my_instance(), uMAKEINTRESOURCE(id), "TEXT");
		return { static_cast<const char*>(res->GetPointer()), res->GetSize() };
	}

	static std::string print_guid(const GUID& g)
	{
		return fmt::format("{{{}}}", pfc::print_guid(g).get_ptr());
	}
}
