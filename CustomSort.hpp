#pragma once

namespace CustomSort
{
	struct Item
	{
		std::wstring text;
		size_t index{};
	};

	using Order = pfc::array_t<size_t>;

	template <int direction>
	static bool sort_compare(const Item& a, const Item& b)
	{
		const int ret = direction * StrCmpLogicalW(a.text.data(), b.text.data());
		if (ret == 0) return a.index < b.index;
		return ret < 0;
	}

	static Order order(size_t count)
	{
		Order sort_order;
		sort_order.set_size(count);
		std::iota(sort_order.begin(), sort_order.end(), size_t{});
		return sort_order;
	}

	static Order sort(pfc::array_t<Item>& items, int direction = 1)
	{
		auto sort_order = order(items.get_count());
		std::ranges::sort(items, direction > 0 ? sort_compare<1> : sort_compare<-1>);
		std::ranges::transform(items, sort_order.begin(), [](const Item& item) { return item.index; });
		return sort_order;
	}
}
