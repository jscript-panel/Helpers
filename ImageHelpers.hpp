#pragma once

class ImageHelpers
{
public:
	static HRESULT data_to_bitmap(const uint8_t* data, size_t bytes, wil::com_ptr_t<IWICBitmap>& bitmap);
	static HRESULT istream_to_image_data(IStream* stream, std::vector<uint8_t>& image_data);
	static HRESULT save_as_jpg(IWICBitmap* bitmap, wil::zwstring_view path);
	static IJSImage* create(uint32_t width, uint32_t height);
	static IJSImage* path_to_image(wil::zwstring_view path);
	static IJSImage* svg_to_image(wil::zwstring_view path_or_xml, float max_width);

private:
	static HRESULT istream_to_bitmap(IStream* stream, wil::com_ptr_t<IWICBitmap>& bitmap);
	static HRESULT libwebp_data_to_bitmap(const uint8_t* data, size_t bytes, wil::com_ptr_t<IWICBitmap>& bitmap);
	static HRESULT libwebp_istream_to_bitmap(IStream* stream, wil::com_ptr_t<IWICBitmap>& bitmap);
};
