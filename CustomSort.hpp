#pragma once

namespace CustomSort
{
	struct Item
	{
		std::wstring text;
		size_t index{};
	};

	using Items = pfc::array_t<Item>;
	using Order = pfc::array_t<size_t>;

	template <int direction>
	static bool sort_compare(const Item& a, const Item& b)
	{
		const int ret = direction * StrCmpLogicalW(a.text.data(), b.text.data());
		if (ret == 0) return a.index < b.index;
		return ret < 0;
	}

	static Items items(size_t count)
	{
		Items items;
		items.set_size(count);
		return items;
	}

	static Order sort(Items& items, int direction = 1)
	{
		Order order;
		order.set_size(items.size());
		std::ranges::sort(items, direction > 0 ? sort_compare<1> : sort_compare<-1>);
		std::ranges::transform(items, order.begin(), [](const Item& item) { return item.index; });
		return order;
	}

	static Order order(size_t count)
	{
		Order order;
		order.set_size(count);
		std::iota(order.begin(), order.end(), 0U);
		return order;
	}
}
