#pragma once

class FileHelper
{
public:
	FileHelper(std::wstring_view path);
	FileHelper(std::string_view path);

	static bool rename(std::wstring_view from, std::wstring_view to);

	WStrings list_files(bool recur = false);
	WStrings list_folders(bool recur = false);
	bool copy_file(std::wstring_view to, bool overwrite);
	bool copy_folder(std::wstring_view to, bool overwrite, bool recur);
	bool create_folder();
	bool is_file();
	bool is_folder();
	bool remove();
	bool remove_folder_recursive(uint32_t options);
	bool write(const void* data, size_t size);
	bool write(std::string_view content);
	bool write(std::wstring_view content);
	std::string read();
	std::wstring filename();
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

	static uint32_t guess_codepage(std::string_view content);

	template <typename DirectoryIterator = std::filesystem::directory_iterator>
	WStrings list_t(EntryType type);

	static constexpr std::string_view UTF_16_LE_BOM = "\xFF\xFE";
	static constexpr std::string_view UTF_8_BOM = "\xEF\xBB\xBF";

	std::filesystem::path m_path;
};
