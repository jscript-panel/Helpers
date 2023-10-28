#include "stdafx.hpp"

namespace
{
	class FontHelperImpl : public FontHelper
	{
	public:
		WStrings get_names() final
		{
			init();
			return m_names;
		}

		bool check_name(wil::zwstring_view name_to_check) final
		{
			init();
			const auto it = std::ranges::find_if(m_names, [name_to_check](wil::zwstring_view name)
				{
					return _wcsicmp(name.data(), name_to_check.data()) == 0;
				});

			return it != m_names.end();
		}

		std::string logfont_to_string(const LOGFONT& lf) final
		{
			auto j = JSON::object();
			j["Size"] = std::abs(lf.lfHeight);

			const auto hr = [&]
				{
					wil::com_ptr_t<IDWriteFont> font;
					wil::com_ptr_t<IDWriteFontFamily> font_family;

					RETURN_IF_FAILED(g_gdi_interop->CreateFontFromLOGFONT(&lf, &font));
					RETURN_IF_FAILED(font->GetFontFamily(&font_family));
					const std::wstring name = get_family_name(font_family);

					j["Name"] = from_wide(name);
					j["Weight"] = std::to_underlying(font->GetWeight());
					j["Style"] = std::to_underlying(font->GetStyle());
					j["Stretch"] = std::to_underlying(font->GetStretch());
					return S_OK;
				}();

			if FAILED(hr)
			{
				j["Name"] = from_wide(s_default);
				j["Weight"] = lf.lfWeight;
				j["Style"] = lf.lfItalic != 0 ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL;
			}

			return j.dump(4);
		}

	private:
		std::wstring get_family_name(const wil::com_ptr_t<IDWriteFontFamily>& font_family)
		{
			wil::com_ptr_t<IDWriteLocalizedStrings> family_names;
			if SUCCEEDED(font_family->GetFamilyNames(&family_names))
			{
				FontName name{};
				if SUCCEEDED(family_names->GetString(0, name.data(), FontNameSize))
				{
					return name.data();
				}
			}
			return s_default.data();
		}

		void init()
		{
			if (m_names.size()) return;

			wil::com_ptr_t<IDWriteFontCollection> font_collection;
			wil::com_ptr_t<IDWriteFontFamily> font_family;

			if FAILED(g_dwrite_factory->GetSystemFontCollection(&font_collection, TRUE)) return;

			const uint32_t family_count = font_collection->GetFontFamilyCount();
			for (const uint32_t i : std::views::iota(0U, family_count))
			{
				if FAILED(font_collection->GetFontFamily(i, &font_family)) continue;
				const std::wstring name = get_family_name(font_family);
				m_names.emplace_back(name);
			}

			std::ranges::sort(m_names, [](wil::zwstring_view a, wil::zwstring_view b)
				{
					return StrCmpLogicalW(a.data(), b.data()) < 0;
				});
		}

		static constexpr uint32_t FontNameSize = 100U;
		using FontName = std::array<wchar_t, FontNameSize>;

		WStrings m_names;
	};

	FB2K_SERVICE_FACTORY(FontHelperImpl)
}
