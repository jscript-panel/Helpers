#pragma once

namespace js
{
	HRESULT check_stream_size(IStream* stream);
	HRESULT fit_to(uint32_t max_size, wil::com_ptr_t<IWICBitmap>& bitmap);
	HRESULT istream_to_bitmap(IStream* stream, wil::com_ptr_t<IWICBitmap>& bitmap);
	HRESULT libwebp_data_to_bitmap(const uint8_t* data, size_t data_size, wil::com_ptr_t<IWICBitmap>& bitmap);
	HRESULT libwebp_istream_to_bitmap(IStream* stream, wil::com_ptr_t<IWICBitmap>& bitmap);
	HRESULT path_to_istream(std::wstring_view path, wil::com_ptr_t<IStream>& stream);
	HRESULT resize_bitmap(uint32_t width, uint32_t height, wil::com_ptr_t<IWICBitmap>& bitmap);
	IJSImage* create_image(uint32_t width, uint32_t height);
	IJSImage* path_to_image(std::wstring_view path, uint32_t max_size = 0U);
	IJSImage* svg_to_image(std::wstring_view path_or_xml, float max_width);
	bool save_as_jpg(IWICBitmap* bitmap, std::wstring_view path);
	uint32_t get_stream_size(IStream* stream);
}
