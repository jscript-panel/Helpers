#pragma once

using WStrings = std::vector<std::wstring>;

static constexpr std::wstring_view TAB = L"\t";
static constexpr std::wstring_view BEL = L"\7";
static constexpr std::wstring_view ETX = L"\3";

namespace js
{
	static WStrings split_string(std::wstring_view text, std::wstring_view delims)
	{
		return text | std::views::split(delims) | std::ranges::to<WStrings>();
	}

	static bool compare_string(std::wstring_view a, std::wstring_view b)
	{
		return _wcsicmp(a.data(), b.data()) == 0;
	}

	// based on SDK/titleformat/remove_color_marks
	static std::wstring remove_mark(std::wstring_view str, std::wstring_view mark)
	{
		if (!str.contains(mark))
			return { str.data(), str.size() };

		auto src = str.data();
		const auto ch = mark.at(0);

		std::wstring ret;

		while (*src)
		{
			if (*src == ch)
			{
				src++;
				while (*src && *src != ch)
				{
					src++;
				}

				if (*src == ch)
				{
					src++;
				}
			}
			else
			{
				ret += *src++;
			}
		}

		return ret;
	}

	static std::wstring remove_marks(std::wstring_view str)
	{
		std::wstring ret = remove_mark(str, BEL);
		ret = remove_mark(ret, ETX);
		return ret;
	}

	static uint32_t lengthu(std::wstring_view str)
	{
		return to_uint(str.length());
	}

	static void sort_strings(WStrings& strings)
	{
		std::ranges::sort(strings, [](auto&& lhs, auto&& rhs)
			{
				return StrCmpLogicalW(lhs.data(), rhs.data()) < 0;
			});
	}
}
