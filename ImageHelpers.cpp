#include "stdafx.hpp"

#ifdef NDEBUG
resvg_options* g_resvg_font_options{};

void create_resvg_font_options()
{
	g_resvg_font_options = resvg_options_create();
	resvg_options_load_system_fonts(g_resvg_font_options);
}

void destroy_resvg_font_options()
{
	if (g_resvg_font_options != nullptr)
	{
		resvg_options_destroy(g_resvg_font_options);
		g_resvg_font_options = nullptr;
	}
}
#endif

HRESULT ImageHelpers::album_art_data_to_bitmap(const AlbumArt::Data& data, wil::com_ptr_t<IWICBitmap>& bitmap)
{
	RETURN_HR_IF(E_FAIL, data.is_empty());

	auto ptr = static_cast<const uint8_t*>(data->data());
	const size_t data_size = data->size();

	if SUCCEEDED(libwebp_data_to_bitmap(ptr, data_size, bitmap)) return S_OK;

	auto str = SHCreateMemStream(ptr, to_uint(data_size));
	RETURN_HR_IF_NULL(E_FAIL, str);

	wil::com_ptr_t<IStream> stream;
	stream.attach(str);

	RETURN_IF_FAILED(istream_to_bitmap(stream.get(), bitmap));
	return S_OK;
}

HRESULT ImageHelpers::istream_to_bitmap(IStream* stream, wil::com_ptr_t<IWICBitmap>& bitmap)
{
	wil::com_ptr_t<IWICBitmapDecoder> decoder;
	wil::com_ptr_t<IWICBitmapFrameDecode> frame_decode;
	wil::com_ptr_t<IWICBitmapSource> source;

	RETURN_IF_FAILED(g_imaging_factory->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnDemand, &decoder));
	RETURN_IF_FAILED(decoder->GetFrame(0, &frame_decode));
	RETURN_IF_FAILED(WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, frame_decode.get(), &source));
	RETURN_IF_FAILED(g_imaging_factory->CreateBitmapFromSource(source.get(), WICBitmapCacheOnDemand, &bitmap));
	return S_OK;
}

HRESULT ImageHelpers::libwebp_data_to_bitmap(const uint8_t* data, size_t data_size, wil::com_ptr_t<IWICBitmap>& bitmap)
{
	WebPBitstreamFeatures bs;
	if (WebPGetFeatures(data, data_size, &bs) == VP8_STATUS_OK && !bs.has_animation)
	{
		auto webp = WebPDecodeBGRA(data, data_size, &bs.width, &bs.height);
		if (webp)
		{
			const auto width = to_uint(bs.width);
			const auto height = to_uint(bs.height);
			const auto stride = width * 4U;
			const auto size = stride * height;
			const HRESULT hr = g_imaging_factory->CreateBitmapFromMemory(width, height, GUID_WICPixelFormat32bppPBGRA, stride, size, webp, &bitmap);
			WebPFree(webp);
			return hr;
		}
	}
	return E_FAIL;
}

HRESULT ImageHelpers::libwebp_istream_to_bitmap(IStream* stream, wil::com_ptr_t<IWICBitmap>& bitmap)
{
	auto data = AlbumArt::istream_to_data(stream);
	RETURN_HR_IF(E_FAIL, data.is_empty());
	RETURN_IF_FAILED(libwebp_data_to_bitmap(static_cast<const uint8_t*>(data->data()), data->size(), bitmap));
	return S_OK;
}

HRESULT ImageHelpers::fit_to(uint32_t max_size, wil::com_ptr_t<IWICBitmap>& bitmap)
{
	if (max_size == 0U) return S_OK;

	uint32_t old_width{}, old_height{};
	RETURN_IF_FAILED(bitmap->GetSize(&old_width, &old_height));
	if (old_width <= max_size && old_height <= max_size) return S_OK;

	if (old_width == old_height)
	{
		return resize(max_size, max_size, bitmap);
	}

	const double dmax = static_cast<double>(max_size);
	const double dw = static_cast<double>(old_width);
	const double dh = static_cast<double>(old_height);
	const double s = std::min(dmax / dw, dmax / dh);
	const uint32_t new_width = to_uint(dw * s);
	const uint32_t new_height = to_uint(dh * s);

	return resize(new_width, new_height, bitmap);
}

HRESULT ImageHelpers::resize(uint32_t width, uint32_t height, wil::com_ptr_t<IWICBitmap>& bitmap)
{
	wil::com_ptr_t<IWICBitmapScaler> scaler;
	RETURN_IF_FAILED(g_imaging_factory->CreateBitmapScaler(&scaler));
	RETURN_IF_FAILED(scaler->Initialize(bitmap.get(), width, height, WICBitmapInterpolationModeFant));
	RETURN_IF_FAILED(g_imaging_factory->CreateBitmapFromSource(scaler.get(), WICBitmapCacheOnDemand, &bitmap));
	return S_OK;
}

HRESULT ImageHelpers::save_as_jpg(IWICBitmap* bitmap, wil::zwstring_view path)
{
	uint32_t width{}, height{};
	wil::com_ptr_t<IWICBitmapEncoder> encoder;
	wil::com_ptr_t<IWICBitmapFrameEncode> frame_encode;
	wil::com_ptr_t<IWICStream> stream;

	RETURN_IF_FAILED(bitmap->GetSize(&width, &height));
	WICRect rect(0, 0, to_int(width), to_int(height));

	RETURN_IF_FAILED(g_imaging_factory->CreateStream(&stream));
	RETURN_IF_FAILED(stream->InitializeFromFilename(path.data(), GENERIC_WRITE));
	RETURN_IF_FAILED(g_imaging_factory->CreateEncoder(GUID_ContainerFormatJpeg, nullptr, &encoder));
	RETURN_IF_FAILED(encoder->Initialize(stream.get(), WICBitmapEncoderNoCache));
	RETURN_IF_FAILED(encoder->CreateNewFrame(&frame_encode, nullptr));
	RETURN_IF_FAILED(frame_encode->Initialize(nullptr));
	RETURN_IF_FAILED(frame_encode->SetSize(width, height));
	RETURN_IF_FAILED(frame_encode->WriteSource(bitmap, &rect));
	RETURN_IF_FAILED(frame_encode->Commit());
	RETURN_IF_FAILED(encoder->Commit());
	return S_OK;
}

IJSImage* ImageHelpers::create(uint32_t width, uint32_t height)
{
	wil::com_ptr_t<IWICBitmap> bitmap;
	if FAILED(g_imaging_factory->CreateBitmap(width, height, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnDemand, &bitmap)) return nullptr;
	return new ComObject<JSImage>(bitmap);
}

IJSImage* ImageHelpers::path_to_image(wil::zwstring_view path)
{
	wil::com_ptr_t<IStream> stream;
	if FAILED(SHCreateStreamOnFileEx(path.data(), STGM_READ | STGM_SHARE_DENY_WRITE, GENERIC_READ, FALSE, nullptr, &stream)) return nullptr;

	wil::com_ptr_t<IWICBitmap> bitmap;
	HRESULT hr = istream_to_bitmap(stream.get(), bitmap);
	if FAILED(hr) hr = libwebp_istream_to_bitmap(stream.get(), bitmap);
	if FAILED(hr) return nullptr;
	return new ComObject<JSImage>(bitmap, path);
}

IJSImage* ImageHelpers::svg_to_image([[maybe_unused]] wil::zwstring_view path_or_xml, [[maybe_unused]] float max_width)
{
#ifdef NDEBUG
	std::string xml;
	if (path_or_xml.contains(L'<'))
	{
		xml = from_wide(path_or_xml);
	}
	else
	{
		xml = FileHelper(path_or_xml).read();
		if (xml.empty()) return nullptr;
	}

	int status{};
	resvg_render_tree* tree{};

	if (xml.contains("<text"))
	{
		static std::once_flag once;
		std::call_once(once, create_resvg_font_options);
		status = resvg_parse_tree_from_data(xml.c_str(), xml.length(), g_resvg_font_options, &tree);
	}
	else
	{
		resvg_options* options = resvg_options_create();
		status = resvg_parse_tree_from_data(xml.c_str(), xml.length(), options, &tree);
		resvg_options_destroy(options);
	}

	if (status == RESVG_OK)
	{
		const auto svg_size = resvg_get_image_size(tree);
		float ratio = 1.f;
		uint32_t width{}, height{};

		if (max_width == 0.f)
		{
			width = to_uint(svg_size.width);
			height = to_uint(svg_size.height);
		}
		else
		{
			ratio = max_width / svg_size.width;
			width = to_uint(max_width);
			height = to_uint(svg_size.height / svg_size.width * max_width);
		}

		const auto transform = resvg_transform(ratio, 0.f, 0.f, ratio, 0.f, 0.f);
		const auto stride = width * 4U;
		const auto size = stride * height;
		std::vector<uint8_t> data(size);
		wil::com_ptr_t<IWICBitmap> bitmap;

		resvg_render(tree, transform, width, height, reinterpret_cast<char*>(data.data()));
		resvg_tree_destroy(tree);
		tree = nullptr;

		if SUCCEEDED(g_imaging_factory->CreateBitmapFromMemory(width, height, GUID_WICPixelFormat32bppPRGBA, stride, size, data.data(), &bitmap))
		{
			return new ComObject<JSImage>(bitmap);
		}
	}
#endif
	return nullptr;
}
