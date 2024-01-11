#pragma once

using StringMap = std::map<std::string, std::string>;
using StringPair = std::pair<std::string, std::string>;
using Strings = std::vector<std::string>;
using WStrings = std::vector<std::wstring>;
using pfc::string8;

static constexpr wil::zstring_view CRLF = "\r\n";

BSTR to_bstr(std::string_view str);
Strings split_string(std::string_view text, std::string_view delims);
bool compare_string(wil::zstring_view a, wil::zstring_view b);
bool compare_string(wil::zwstring_view a, wil::zwstring_view b);
std::string from_wide(std::wstring_view str);
std::string print_guid(const GUID& g);
std::wstring to_wide(std::string_view str);
std::wstring wdisplay_path(wil::zstring_view path);
void sort_strings(WStrings& strings);
