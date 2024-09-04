#include "stdafx.hpp"
#include "FileHelper.hpp"

namespace fs = std::filesystem;

FileHelper::FileHelper(std::wstring_view path) : m_path(path.data()) {}
FileHelper::FileHelper(std::string_view path) : m_path(js::to_wide(path)) {}

#pragma region static
bool FileHelper::rename(std::wstring_view from, std::wstring_view to)
{
	const auto fs_from = fs::path(from.data());
	const auto fs_to = fs::path(to.data());
	std::error_code ec;

	fs::rename(fs_from, fs_to, ec);
	return ec.value() == 0;
}
#pragma endregion

HRESULT FileHelper::read(wil::com_ptr_t<IStream>& stream)
{
	RETURN_IF_FAILED(SHCreateStreamOnFileEx(m_path.c_str(), STGM_READ | STGM_SHARE_DENY_WRITE, GENERIC_READ, FALSE, nullptr, &stream));
	RETURN_IF_FAILED(js::check_stream_size(stream.get()));
	return S_OK;
}

WStrings FileHelper::list_files(bool recur)
{
	if (recur)
		return list_t<fs::recursive_directory_iterator>(EntryType::File);

	return list_t(EntryType::File);
}

WStrings FileHelper::list_folders(bool recur)
{
	if (recur)
		return list_t<fs::recursive_directory_iterator>(EntryType::Folder);

	return list_t(EntryType::Folder);
}

template <typename DirectoryIterator>
WStrings FileHelper::list_t(EntryType type)
{
	WStrings paths;

	if (is_folder())
	{
		for (const fs::directory_entry& entry : DirectoryIterator(m_path, fs::directory_options::skip_permission_denied))
		{
			if (type == EntryType::File && entry.is_regular_file())
			{
				paths.emplace_back(entry.path().native());
			}
			else if (type == EntryType::Folder && entry.is_directory())
			{
				paths.emplace_back(entry.path().native() + fs::path::preferred_separator);
			}
		}

		js::sort_strings(paths);
	}

	return paths;
}

bool FileHelper::copy_file(std::wstring_view to, bool overwrite)
{
	if (!is_file())
		return false;
	
	fs::copy_options options{};

	if (overwrite)
	{
		options |= fs::copy_options::overwrite_existing;
	}

	const auto fs_to = fs::path(to.data());
	std::error_code ec;
	return fs::copy_file(m_path, fs_to, options, ec);
}

bool FileHelper::copy_folder(std::wstring_view to, bool overwrite, bool recur)
{
	if (!is_folder())
		return false;

	fs::copy_options options{};

	if (overwrite)
	{
		options |= fs::copy_options::overwrite_existing;
	}
	
	if (recur)
	{
		options |= fs::copy_options::recursive;
	}

	const auto fs_to = fs::path(to.data());
	std::error_code ec;
	fs::copy(m_path, fs_to, options, ec);
	return ec.value() == 0;
}

bool FileHelper::create_folder()
{
	std::error_code ec;
	if (fs::is_directory(m_path, ec))
		return true;

	return fs::create_directories(m_path, ec);
}

bool FileHelper::is_file()
{
	std::error_code ec;
	return fs::is_regular_file(m_path, ec);
}

bool FileHelper::is_folder()
{
	std::error_code ec;
	return fs::is_directory(m_path, ec);
}

bool FileHelper::remove()
{
	std::error_code ec;
	return fs::remove(m_path, ec);
}

bool FileHelper::remove_folder_recursive(uint32_t options)
{
	if (!is_folder())
		return false;

	const auto options_enum = static_cast<wil::RemoveDirectoryOptions>(options);
	return SUCCEEDED(wil::RemoveDirectoryRecursiveNoThrow(m_path.c_str(), options_enum));
}

bool FileHelper::write(const void* data, size_t size)
{
	auto f = std::ofstream(m_path, std::ios::binary);
	if (!f.is_open())
		return false;
	
	return f.write((char*)data, size).good();
}

uint64_t FileHelper::file_size()
{
	std::error_code ec;
	if (!fs::is_regular_file(m_path, ec))
		return {};

	return fs::file_size(m_path, ec);
}

uint64_t FileHelper::last_modified()
{
	std::error_code ec;
	const auto last = fs::last_write_time(m_path, ec);
	if (ec.value() != 0)
		return {};
	
	const auto windows_time = static_cast<uint64_t>(last.time_since_epoch().count());
	return pfc::fileTimeWtoU(windows_time);
}
