#pragma once

class NOVTABLE FontHelper : public service_base
{
public:
	static auto get()
	{
		return fb2k::std_api_get<FontHelper>();
	}

	virtual WStrings get_names() = 0;
	virtual bool check_name(wil::zwstring_view name_to_check) = 0;
	virtual std::string logfont_to_string(const LOGFONT& lf) = 0;

	static constexpr wil::zwstring_view s_default = L"Segoe UI";

	FB2K_MAKE_SERVICE_INTERFACE_ENTRYPOINT(FontHelper)
};

FOOGUIDDECL constexpr GUID FontHelper::class_guid = guids::font_helper_class;
