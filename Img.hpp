#pragma once

class Img
{
public:
	static HRESULT check_stream_size(IStream* stream);
	static HRESULT fit_to(uint32_t max_size, wil::com_ptr_t<IWICBitmap>& bitmap);
	static HRESULT istream_to_bitmap(IStream* stream, wil::com_ptr_t<IWICBitmap>& bitmap);
	static HRESULT libwebp_data_to_bitmap(const uint8_t* data, size_t data_size, wil::com_ptr_t<IWICBitmap>& bitmap);
	static HRESULT libwebp_istream_to_bitmap(IStream* stream, wil::com_ptr_t<IWICBitmap>& bitmap);
	static HRESULT path_to_istream(wil::zwstring_view path, wil::com_ptr_t<IStream>& stream);
	static HRESULT resize(uint32_t width, uint32_t height, wil::com_ptr_t<IWICBitmap>& bitmap);
	static HRESULT save_as_jpg(IWICBitmap* bitmap, wil::zwstring_view path);
	static IJSImage* create(uint32_t width, uint32_t height);
	static IJSImage* path_to_image(wil::zwstring_view path, uint32_t max_size = 0U);
	static IJSImage* svg_to_image(wil::zwstring_view path_or_xml, float max_width);
	static uint32_t get_stream_size(IStream* stream);
};
