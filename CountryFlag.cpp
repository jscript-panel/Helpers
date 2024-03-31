#include "stdafx.hpp"
#include "CountryFlag.hpp"

CountryFlag::FlagData CountryFlag::get_flag_data()
{
	FlagData flag_data;
	const auto str = Component::get_resource_text(IDR_COUNTRIES_JSON);
	const auto j = JSONHelper::parse(str);

	for (const auto& [key, value] : j.items())
	{
		Country country;
		country.name = value["name"].get<std::string>();
		country.flag = value["flag"].get<std::string>();

		flag_data.emplace(key, country);
	}

	return flag_data;
}

std::string CountryFlag::get(wil::zstring_view country_or_code)
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
				return compare_string(country_or_code, item.second.name);
			});

		if (it != flag_data.end())
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
