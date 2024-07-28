#include "stdafx.hpp"
#include "CountryFlag.hpp"

CountryFlag::FlagData CountryFlag::get_flag_data()
{
	FlagData flag_data;
	const auto str = js::get_resource_text(IDR_COUNTRIES_JSON);
	const auto j = js::json_parse(str);

	for (const auto& [key, value] : j.items())
	{
		Country country;
		country.name = value["name"].get<std::string>();
		country.flag = value["flag"].get<std::string>();

		flag_data.emplace(key, country);
	}

	return flag_data;
}

std::string CountryFlag::get(std::string_view country_or_code)
{
	static const auto flag_data = get_flag_data();

	if (country_or_code.length() == 2)
	{
		const auto lowered = string8(country_or_code).toLower();
		const auto it = flag_data.find(lowered.get_ptr());

		if (it != flag_data.end())
		{
			return it->second.flag;
		}
	}
	else if (country_or_code.length() >= 4)
	{
		const auto it = std::ranges::find_if(flag_data, [&](auto&& item)
			{
				return js::compare_string(country_or_code, item.second.name);
			});

		if (it != flag_data.end())
		{
			return it->second.flag;
		}
	}

	return {};
}

std::wstring CountryFlag::get(std::wstring_view country_or_code)
{
	const auto ucountry_or_code = js::from_wide(country_or_code);
	const auto str = get(ucountry_or_code);
	return js::to_wide(str);
}
