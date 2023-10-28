#include "stdafx.hpp"
#include "JSONHelper.hpp"

HRESULT JSONHelper::check_two_number_array(JSON& j)
{
	if (j.is_array() && j.size() == 2 && j[0].is_number() && j[1].is_number()) return S_OK;
	return E_INVALIDARG;
}

HRESULT JSONHelper::to_dwrite_text_range(JSON& obj, DWRITE_TEXT_RANGE& range)
{
	if (obj.is_object() && obj["Start"].is_number_unsigned() && obj["Length"].is_number_unsigned())
	{
		range.startPosition = obj["Start"].get<uint32_t>();
		range.length = obj["Length"].get<uint32_t>();
		return S_OK;
	}
	return E_INVALIDARG;
}

HRESULT JSONHelper::to_gradient_stops(JSON& jstops, std::vector<D2D1_GRADIENT_STOP>& stops)
{
	if (jstops.is_array() && jstops.size() >= 2)
	{
		for (auto&& jstop : jstops)
		{
			RETURN_IF_FAILED(check_two_number_array(jstop));

			const auto pos = jstop[0].get<float>();
			const auto colour = jstop[1].get<int64_t>();
			const auto stop = D2D1::GradientStop(pos, to_colorf(colour));
			stops.emplace_back(stop);
		}
		return S_OK;
	}
	return E_INVALIDARG;
}

JSON JSONHelper::parse(wil::zstring_view str)
{
	return JSON::parse(str, nullptr, false);
}

JSON JSONHelper::parse(wil::zwstring_view str)
{
	return parse(from_wide(str));
}

Strings JSONHelper::to_strings(JSON j)
{
	if (!j.is_array()) j = JSON::array({ j });

	auto transform = [](auto&& j2) { return to_string(j2); };
	auto filter = [](auto&& str) { return str.length() > 0; };
	return j | std::views::transform(transform) | std::views::filter(filter) | std::ranges::to<Strings>();
}

std::optional<D2D1_POINT_2F> JSONHelper::to_point(JSON& j)
{
	if FAILED(check_two_number_array(j)) return std::nullopt;

	const auto x = j[0].get<float>();
	const auto y = j[1].get<float>();
	return D2D1::Point2F(x, y);
}

std::string JSONHelper::to_string(JSON& j)
{
	if (j.is_null()) return std::string();
	if (j.is_string()) return j.get<std::string>();
	return j.dump();
}
