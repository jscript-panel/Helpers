#pragma once

class FileHelper
{
public:
	FileHelper(wil::zwstring_view path);
	FileHelper(wil::zstring_view path);

	WStrings list_files(bool recur = false);
	WStrings list_folders(bool recur = false);
	bool create_folder();
	bool is_file();
	bool is_folder();
	bool remove();
	bool write(wil::zstring_view content);
	bool write(wil::zwstring_view content);
	std::string read();
	std::wstring filename();
	std::wstring parent_path();
	uint32_t guess_codepage();
	uint64_t file_size();
	uint64_t last_modified();
	void read_wide(uint32_t codepage, std::wstring& content);

private:
	enum class EntryType
	{
		File,
		Folder,
	};

	static uint32_t guess_codepage(wil::zstring_view content);

	template <typename DirectoryIterator = std::filesystem::directory_iterator>
	WStrings list_t(EntryType type);

	static constexpr wil::zstring_view UTF_16_LE_BOM = "\xFF\xFE";
	static constexpr wil::zstring_view UTF_8_BOM = "\xEF\xBB\xBF";

	std::filesystem::path m_path;
};
