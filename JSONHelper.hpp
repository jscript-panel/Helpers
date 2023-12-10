#pragma once

using JSON = nlohmann::json;

class JSONHelper
{
public:
	static HRESULT to_dwrite_text_range(JSON& obj, DWRITE_TEXT_RANGE& range);
	static HRESULT to_dwrite_text_range_and_colour(JSON& obj, DWRITE_TEXT_RANGE& range, D2D1_COLOR_F& colour);
	static HRESULT to_gradient_stops(JSON& jstops, std::vector<D2D1_GRADIENT_STOP>& stops);
	static JSON parse(wil::zstring_view str);
	static JSON parse(wil::zwstring_view str);
	static Strings to_strings(JSON j);
	static std::optional<D2D1_POINT_2F> to_point(JSON& j);
	static std::string to_string(JSON& j);

private:
	static HRESULT check_two_number_array(JSON& j);
};
