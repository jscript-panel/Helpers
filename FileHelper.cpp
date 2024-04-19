#include "stdafx.hpp"
#include "FileHelper.hpp"

namespace fs = std::filesystem;

FileHelper::FileHelper(wil::zwstring_view path) : m_path(path.data()) {}
FileHelper::FileHelper(wil::zstring_view path) : m_path(js::to_wide(path)) {}

#pragma region static
bool FileHelper::rename(wil::zwstring_view from, wil::zwstring_view to)
{
	const auto fs_from = fs::path(from.data());
	const auto fs_to = fs::path(to.data());
	std::error_code ec;

	fs::rename(fs_from, fs_to, ec);
	return ec.value() == 0;
}

uint32_t FileHelper::guess_codepage(wil::zstring_view content)
{
	if (content.empty()) return 0;

	auto lang = wil::CoCreateInstanceNoThrow<IMultiLanguage2>(CLSID_CMultiLanguage);
	if (!lang) return 0;

	auto src = const_cast<char*>(content.data());
	static constexpr int max_encodings = 1;
	int encoding_count = max_encodings;
	int size = js::to_int(content.length());
	std::array<DetectEncodingInfo, max_encodings> encodings{};
	if FAILED(lang->DetectInputCodepage(MLDETECTCP_NONE, 0, src, &size, encodings.data(), &encoding_count)) return 0;

	const uint32_t codepage = encodings[0].nCodePage;
	if (codepage == 20127) return CP_UTF8;
	return codepage;
}
#pragma endregion

WStrings FileHelper::list_files(bool recur)
{
	if (recur) return list_t<fs::recursive_directory_iterator>(EntryType::File);
	return list_t(EntryType::File);
}

WStrings FileHelper::list_folders(bool recur)
{
	if (recur) return list_t<fs::recursive_directory_iterator>(EntryType::Folder);
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

bool FileHelper::copy_file(wil::zwstring_view to, bool overwrite)
{
	if (is_file())
	{
		fs::copy_options options{};
		if (overwrite) options |= fs::copy_options::overwrite_existing;

		const auto fs_to = fs::path(to.data());
		std::error_code ec;
		return fs::copy_file(m_path, fs_to, options, ec);
	}

	return false;
}

bool FileHelper::copy_folder(wil::zwstring_view to, bool overwrite, bool recur)
{
	if (is_folder())
	{
		fs::copy_options options{};
		if (overwrite) options |= fs::copy_options::overwrite_existing;
		if (recur) options |= fs::copy_options::recursive;

		const auto fs_to = fs::path(to.data());
		std::error_code ec;
		fs::copy(m_path, fs_to, options, ec);
		return ec.value() == 0;
	}

	return false;
}

bool FileHelper::create_folder()
{
	std::error_code ec;
	if (fs::is_directory(m_path, ec)) return true;
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
	if (is_folder())
	{
		const auto options_enum = static_cast<wil::RemoveDirectoryOptions>(options);
		return SUCCEEDED(wil::RemoveDirectoryRecursiveNoThrow(m_path.c_str(), options_enum));
	}
	return false;
}

bool FileHelper::write(const void* data, size_t size)
{
	auto f = std::ofstream(m_path, std::ios::binary);

	if (f.is_open())
	{
		return f.write((char*)data, size).good();
	}

	return false;
}

bool FileHelper::write(wil::zstring_view content)
{
	auto f = std::ofstream(m_path, std::ios::binary);

	if (f.is_open())
	{
		return f.write(content.data(), content.length()).good();
	}

	return false;
}

bool FileHelper::write(wil::zwstring_view content)
{
	const std::string ucontent = js::from_wide(content);
	return write(ucontent);
}

std::string FileHelper::read()
{
	auto f = std::ifstream(m_path);
	if (!f.is_open()) return std::string();

	Strings strings;
	std::string line;
	while (std::getline(f, line))
	{
		strings.emplace_back(line);
	}

	const std::string str = fmt::format("{}", fmt::join(strings, CRLF.data()));
	if (str.starts_with(UTF_8_BOM))
	{
		return str.substr(3);
	}
	return str;
}

std::wstring FileHelper::filename()
{
	return m_path.stem().native();
}

std::wstring FileHelper::parent_path()
{
	return m_path.parent_path().native() + fs::path::preferred_separator;
}

uint32_t FileHelper::guess_codepage()
{
	return guess_codepage(read());
}

uint64_t FileHelper::file_size()
{
	std::error_code ec;
	uint64_t ret{};

	if (fs::is_regular_file(m_path, ec))
	{
		ret = fs::file_size(m_path, ec);
	}
	return ret;
}

uint64_t FileHelper::last_modified()
{
	std::error_code ec;
	uint64_t ret{};

	const auto last = fs::last_write_time(m_path, ec);
	if (ec.value() == 0)
	{
		const auto windows_time = static_cast<uint64_t>(last.time_since_epoch().count());
		ret = pfc::fileTimeWtoU(windows_time);
	}
	return ret;
}

void FileHelper::read_wide(uint32_t codepage, std::wstring& content)
{
	const auto& file = wil::try_open_file(m_path.c_str()).file;
	if (!file) return;

	const auto file_size = GetFileSize(file.get(), nullptr);
	if (file_size == INVALID_FILE_SIZE) return;

	const auto file_mapping = wil::unique_handle(CreateFileMappingW(file.get(), nullptr, PAGE_READONLY, 0, 0, nullptr));
	if (!file_mapping) return;

	const auto ptr = wil::unique_mapview_ptr<uint8_t>(static_cast<uint8_t*>(MapViewOfFile(file_mapping.get(), FILE_MAP_READ, 0, 0, 0)));
	if (!ptr) return;

	if (file_size >= 2 && memcmp(ptr.get(), UTF_16_LE_BOM.data(), 2) == 0)
	{
		content = std::wstring(reinterpret_cast<const wchar_t*>(ptr.get() + 2), (file_size - 2) >> 1);
	}
	else
	{
		const auto str = std::string(reinterpret_cast<const char*>(ptr.get()), file_size);

		if (str.starts_with(UTF_8_BOM))
		{
			content = js::to_wide(str.substr(3));
		}
		else if (codepage == CP_UTF8 || guess_codepage(str) == CP_UTF8)
		{
			content = js::to_wide(str);
		}
		else
		{
			content.resize(pfc::stringcvt::estimate_codepage_to_wide(codepage, str.data(), str.length()));
			pfc::stringcvt::convert_codepage_to_wide(codepage, content.data(), content.length(), str.data(), str.length());
		}
	}
}
