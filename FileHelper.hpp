#pragma once

class FileHelper
{
public:
	FileHelper(std::wstring_view path);
	FileHelper(std::string_view path);

	static bool rename(std::wstring_view from, std::wstring_view to);

	HRESULT read(wil::com_ptr_t<IStream>& stream);
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
	uint64_t file_size();
	uint64_t last_modified();

private:
	enum class EntryType
	{
		File,
		Folder,
	};

	template <typename DirectoryIterator = std::filesystem::directory_iterator>
	WStrings list_t(EntryType type);

	std::filesystem::path m_path;
};
