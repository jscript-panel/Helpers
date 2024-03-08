#include "stdafx.hpp"
#include "JSONHelper.hpp"

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

JSON JSONHelper::parse(wil::zstring_view str)
{
	return JSON::parse(str, nullptr, false);
}

JSON JSONHelper::parse(wil::zwstring_view str)
{
	const auto ustr = from_wide(str);
	return parse(ustr);
}

Strings JSONHelper::to_strings(JSON j)
{
	if (!j.is_array()) j = JSON::array({ j });

	auto transform = [](auto&& j2) { return to_string(j2); };
	auto filter = [](auto&& str) { return str.length() > 0; };
	return j | std::views::transform(transform) | std::views::filter(filter) | std::ranges::to<Strings>();
}

std::string JSONHelper::to_string(JSON& j)
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
