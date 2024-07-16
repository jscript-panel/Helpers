#pragma once

class CountryFlag
{
public:
	static std::string get(std::string_view country_or_code);
	static std::wstring get(std::wstring_view country_or_code);

private:
	struct Country
	{
		std::string name, flag;
	};

	using FlagData = std::map<std::string, Country>;

	static FlagData get_flag_data();
};
