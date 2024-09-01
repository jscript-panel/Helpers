#pragma once

class TextFile
{
public:
	TextFile(std::wstring_view path);
	TextFile(std::string_view path);

	bool write(std::string_view content);
	bool write(std::wstring_view content);
	std::string read();
	uint32_t guess_codepage();
	void read_wide(uint32_t codepage, std::wstring& content);

private:
	static uint32_t guess_codepage(std::string_view content);

	static constexpr std::string_view UTF_16_LE_BOM = "\xFF\xFE";
	static constexpr std::string_view UTF_8_BOM = "\xEF\xBB\xBF";

	std::filesystem::path m_path;
};
