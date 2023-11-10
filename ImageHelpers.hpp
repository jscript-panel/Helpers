#pragma once

class ImageHelpers
{
public:
	static HRESULT album_art_data_to_bitmap(const AlbumArt::Data& data, wil::com_ptr_t<IWICBitmap>& bitmap);
	static HRESULT fit_to(uint32_t max_size, wil::com_ptr_t<IWICBitmap>& bitmap);
	static HRESULT resize(uint32_t width, uint32_t height, wil::com_ptr_t<IWICBitmap>& bitmap);
	static HRESULT save_as_jpg(IWICBitmap* bitmap, wil::zwstring_view path);
	static IJSImage* create(uint32_t width, uint32_t height);
	static IJSImage* path_to_image(wil::zwstring_view path);
	static IJSImage* svg_to_image(wil::zwstring_view path_or_xml, float max_width);

private:
	static HRESULT istream_to_bitmap(IStream* stream, wil::com_ptr_t<IWICBitmap>& bitmap);
	static HRESULT libwebp_data_to_bitmap(const uint8_t* data, size_t data_size, wil::com_ptr_t<IWICBitmap>& bitmap);
	static HRESULT libwebp_istream_to_bitmap(IStream* stream, wil::com_ptr_t<IWICBitmap>& bitmap);
};
