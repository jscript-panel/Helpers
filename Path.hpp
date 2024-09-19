#pragma once

class Path
{
public:
	static std::string now_playing()
	{
		metadb_handle_ptr handle;
		if (playback_control::get()->get_now_playing(handle))
			return handle->get_path();

		return {};
	}

	static std::string replace_illegal_chars(std::string_view str, bool modern)
	{
		if (modern)
			return pfc::io::path::replaceIllegalNameChars(str.data(), false, pfc::io::path::charReplaceModern).get_ptr();

		return pfc::io::path::replaceIllegalNameChars(str.data()).get_ptr();
	}

	static std::string strip_trailing_periods(std::string_view str)
	{
		size_t len = str.length();

		for (const char c : str | std::views::reverse)
		{
			if (c != '.')
				break;

			len--;
		}

		return std::string(str.substr(size_t{}, len));
	}

	static std::wstring filename(std::wstring_view path)
	{
		const auto fs_path = std::filesystem::path(path.data());
		return fs_path.stem().native();
	}

	static std::wstring parent(std::wstring_view path)
	{
		const auto fs_path = std::filesystem::path(path.data());
		return fs_path.parent_path().native() + std::filesystem::path::preferred_separator;
	}

	static std::wstring component()
	{
		const auto path = wil::GetModuleFileNameW(core_api::get_my_instance());
		return parent(path.get());
	}

	static std::wstring fb2k()
	{
		const auto path = wil::GetModuleFileNameW();
		return parent(path.get());
	}

	static std::wstring wdisplay(std::string_view path)
	{
		string8 tmp;
		filesystem::g_get_display_path(path.data(), tmp);
		return js::to_wide(tmp);
	}

	static std::wstring profile()
	{
		return wdisplay(core_api::get_profile_path()) + std::filesystem::path::preferred_separator;
	}
};
