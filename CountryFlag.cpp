#include "stdafx.hpp"
#include "CountryFlag.hpp"

std::string CountryFlag::get(wil::zstring_view country_or_code)
{
	init_flag_data();

	if (country_or_code.length() == 2)
	{
		const auto lowered = string8(country_or_code).toLower();
		const auto it = s_flag_data.find(lowered.get_ptr());

		if (it != s_flag_data.end())
		{
			return it->second.flag;
		}
	}
	else if (country_or_code.length() >= 4)
	{
		const auto it = std::ranges::find_if(s_flag_data, [&](auto&& item)
			{
				return compare_string(country_or_code, item.second.name);
			});

		if (it != s_flag_data.end())
		{
			return it->second.flag;
		}
	}

	return std::string();
}

std::wstring CountryFlag::get(wil::zwstring_view country_or_code)
{
	const auto ucountry_or_code = from_wide(country_or_code);
	const auto str = get(ucountry_or_code);
	return to_wide(str);
}

void CountryFlag::init_flag_data()
{
	if (s_flag_data.empty())
	{
		const auto str = Component::get_resource_text(IDR_COUNTRIES_JSON);
		const auto j = JSONHelper::parse(str);

		for (const auto& [key, value] : j.items())
		{
			Country country;
			country.name = value["name"].get<std::string>();
			country.flag = value["flag"].get<std::string>();

			s_flag_data.emplace(key, country);
		}
	}
}
