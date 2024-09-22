#pragma once

using VariantArgs = std::vector<_variant_t>;

namespace js
{
	template <typename T>
	static auto pfc_array(size_t count)
	{
		pfc::array_t<T> arr;
		arr.set_size(count);
		return arr;
	}

	template <typename T>
	static auto pfc_list(const T& item)
	{
		return pfc::list_single_ref_t<T>(item);
	}

	static void** arg_helper(auto arg)
	{
		return reinterpret_cast<void**>(arg);
	}

	template <typename T>
	static uint32_t sizeu(const T& blah)
	{
		return to_uint(blah.size());
	}

	static JSON json_parse(std::string_view str)
	{
		return JSON::parse(str, nullptr, false);
	}

	static JSON json_parse(std::wstring_view str)
	{
		const auto ustr = from_wide(str);
		return json_parse(ustr);
	}

	static std::string json_to_string(JSON& j)
	{
		if (j.is_string())
			return j.get<std::string>();
		else if (j.is_number())
			return j.dump();
		else
			return {};
	}

	static std::wstring json_to_wstring(JSON& j)
	{
		const auto str = json_to_string(j);
		return to_wide(str);
	}

	static Strings json_to_strings(JSON j)
	{
		if (!j.is_array())
		{
			j = JSON::array({ j });
		}

		auto transform = [](auto&& j2) { return json_to_string(j2); };
		auto filter = [](auto&& str) { return str.length() > 0; };
		return j | std::views::transform(transform) | std::views::filter(filter) | std::ranges::to<Strings>();
	}
}
