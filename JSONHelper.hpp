#pragma once

using JSON = nlohmann::json;

class JSONHelper
{
public:
	static HRESULT to_dwrite_text_range(JSON& obj, DWRITE_TEXT_RANGE& range);
	static JSON parse(wil::zstring_view str);
	static JSON parse(wil::zwstring_view str);
	static Strings to_strings(JSON j);
	static std::string to_string(JSON& j);
};
