#include "stdafx.hpp"
#include "TextFile.hpp"

TextFile::TextFile(std::wstring_view path) : m_path(path.data()) {}
TextFile::TextFile(std::string_view path) : m_path(js::to_wide(path)) {}

#pragma region static
uint32_t TextFile::guess_codepage(std::string_view content)
{
	if (content.empty())
		return 0;

	auto lang = wil::CoCreateInstance<IMultiLanguage2>(CLSID_CMultiLanguage);
	if (!lang)
		return 0;

	auto src = const_cast<char*>(content.data());
	static constexpr int max_encodings = 1;
	int encoding_count = max_encodings;
	int size = js::to_int(content.length());
	std::array<DetectEncodingInfo, max_encodings> encodings{};

	if FAILED(lang->DetectInputCodepage(MLDETECTCP_NONE, 0, src, &size, encodings.data(), &encoding_count))
		return 0;

	const uint32_t codepage = encodings[0].nCodePage;
	if (codepage == 20127)
		return CP_UTF8;

	return codepage;
}
#pragma endregion

bool TextFile::write(std::string_view content)
{
	auto f = std::ofstream(m_path, std::ios::binary);

	if (f.is_open())
	{
		return f.write(content.data(), content.length()).good();
	}

	return false;
}

bool TextFile::write(std::wstring_view content)
{
	const std::string ucontent = js::from_wide(content);
	return write(ucontent);
}

std::string TextFile::read()
{
	auto f = std::ifstream(m_path);
	if (!f.is_open())
		return {};

	Strings strings;
	std::string line;
	while (std::getline(f, line))
	{
		strings.emplace_back(line);
	}

	const std::string str = fmt::format("{}", fmt::join(strings, CRLF.data()));
	if (str.starts_with(UTF_8_BOM))
		return str.substr(3);

	return str;
}

uint32_t TextFile::guess_codepage()
{
	return guess_codepage(read());
}

void TextFile::read_wide(uint32_t codepage, std::wstring& content)
{
	const auto result = wil::try_open_file(m_path.c_str());
	if (result.last_error != 0)
		return;

	const auto file_size = GetFileSize(result.file.get(), nullptr);
	if (file_size == INVALID_FILE_SIZE)
		return;

	const auto file_mapping = wil::unique_handle(CreateFileMappingW(result.file.get(), nullptr, PAGE_READONLY, 0, 0, nullptr));
	if (!file_mapping)
		return;

	const auto ptr = wil::unique_mapview_ptr<uint8_t>(static_cast<uint8_t*>(MapViewOfFile(file_mapping.get(), FILE_MAP_READ, 0, 0, 0)));
	if (!ptr)
		return;

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
