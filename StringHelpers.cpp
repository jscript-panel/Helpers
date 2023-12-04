#include "stdafx.hpp"
#include "StringHelpers.hpp"

BSTR to_bstr(std::string_view str)
{
	const auto wstr = to_wide(str);
	return SysAllocString(wstr.data());
}

Strings split_string(std::string_view text, std::string_view delims)
{
	return text | std::views::split(delims) | std::ranges::to<Strings>();
}

std::string from_wide(std::wstring_view str)
{
	return pfc::utf8FromWide(str.data(), str.length()).get_ptr();
}

std::string print_guid(const GUID& g)
{
	return fmt::format("{{{}}}", pfc::print_guid(g).get_ptr());
}

std::wstring to_wide(std::string_view str)
{
	return pfc::wideFromUTF8(str.data(), str.length()).c_str();
}

std::wstring wdisplay_path(wil::zstring_view path)
{
	string8 tmp;
	filesystem::g_get_display_path(path.data(), tmp);
	return to_wide(tmp);
}
