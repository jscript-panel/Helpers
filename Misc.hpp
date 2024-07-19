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
}
