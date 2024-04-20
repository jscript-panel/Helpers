#include "stdafx.hpp"
#include "Img.hpp"

#ifdef NDEBUG
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "Userenv.lib")

#ifdef _WIN64
#pragma comment(lib, "../../resvg-lib/x64/resvg.lib")
#else
#pragma comment(lib, "../../resvg-lib/Win32/resvg.lib")
#endif

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

HRESULT Img::check_stream_size(IStream* stream)
{
	if (get_stream_size(stream) > Component::max_image_size) return E_INVALIDARG;
	return S_OK;
}

HRESULT Img::fit_to(uint32_t max_size, wil::com_ptr_t<IWICBitmap>& bitmap)
{
	if (max_size == 0U) return S_OK;

	D2D1_SIZE_U size{};
	RETURN_IF_FAILED(bitmap->GetSize(&size.width, &size.height));
	if (size.width <= max_size && size.height <= max_size) return S_OK;

	if (size.width == size.height)
	{
		return resize(max_size, max_size, bitmap);
	}

	const double dmax = static_cast<double>(max_size);
	const double dw = static_cast<double>(size.width);
	const double dh = static_cast<double>(size.height);
	const double s = std::min(dmax / dw, dmax / dh);
	const uint32_t new_width = js::to_uint(dw * s);
	const uint32_t new_height = js::to_uint(dh * s);

	return resize(new_width, new_height, bitmap);
}

HRESULT Img::istream_to_bitmap(IStream* stream, wil::com_ptr_t<IWICBitmap>& bitmap)
{
	wil::com_ptr_t<IWICBitmapDecoder> decoder;
	wil::com_ptr_t<IWICBitmapFrameDecode> frame_decode;
	wil::com_ptr_t<IWICBitmapSource> source;

	RETURN_IF_FAILED(factory::imaging->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnDemand, &decoder));
	RETURN_IF_FAILED(decoder->GetFrame(0, &frame_decode));
	RETURN_IF_FAILED(WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, frame_decode.get(), &source));
	RETURN_IF_FAILED(factory::imaging->CreateBitmapFromSource(source.get(), WICBitmapCacheOnDemand, &bitmap));
	return S_OK;
}

HRESULT Img::libwebp_data_to_bitmap(const uint8_t* data, size_t data_size, wil::com_ptr_t<IWICBitmap>& bitmap)
{
	WebPBitstreamFeatures bs;
	if (WebPGetFeatures(data, data_size, &bs) == VP8_STATUS_OK && !bs.has_animation)
	{
		auto webp = WebPDecodeBGRA(data, data_size, &bs.width, &bs.height);
		if (webp)
		{
			const auto width = js::to_uint(bs.width);
			const auto height = js::to_uint(bs.height);
			const auto stride = width * 4U;
			const auto size = stride * height;
			const HRESULT hr = factory::imaging->CreateBitmapFromMemory(width, height, GUID_WICPixelFormat32bppPBGRA, stride, size, webp, &bitmap);
			WebPFree(webp);
			return hr;
		}
	}
	return E_FAIL;
}

HRESULT Img::libwebp_istream_to_bitmap(IStream* stream, wil::com_ptr_t<IWICBitmap>& bitmap)
{
	auto data = AlbumArtStatic::istream_to_data(stream);
	RETURN_HR_IF(E_FAIL, data.is_empty());
	RETURN_IF_FAILED(libwebp_data_to_bitmap(static_cast<const uint8_t*>(data->data()), data->size(), bitmap));
	return S_OK;
}

HRESULT Img::path_to_istream(wil::zwstring_view path, wil::com_ptr_t<IStream>& stream)
{
	RETURN_IF_FAILED(SHCreateStreamOnFileEx(path.data(), STGM_READ | STGM_SHARE_DENY_WRITE, GENERIC_READ, FALSE, nullptr, &stream));
	RETURN_IF_FAILED(check_stream_size(stream.get()));
	return S_OK;
}

HRESULT Img::resize(uint32_t width, uint32_t height, wil::com_ptr_t<IWICBitmap>& bitmap)
{
	wil::com_ptr_t<IWICBitmapScaler> scaler;
	RETURN_IF_FAILED(factory::imaging->CreateBitmapScaler(&scaler));
	RETURN_IF_FAILED(scaler->Initialize(bitmap.get(), width, height, WICBitmapInterpolationModeFant));
	RETURN_IF_FAILED(factory::imaging->CreateBitmapFromSource(scaler.get(), WICBitmapCacheOnDemand, &bitmap));
	return S_OK;
}

IJSImage* Img::create(uint32_t width, uint32_t height)
{
	wil::com_ptr_t<IWICBitmap> bitmap;
	if FAILED(factory::imaging->CreateBitmap(width, height, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnDemand, &bitmap)) return nullptr;
	return new ComObject<JSImage>(bitmap);
}

IJSImage* Img::path_to_image(wil::zwstring_view path, uint32_t max_size)
{
	wil::com_ptr_t<IStream> stream;
	if FAILED(path_to_istream(path, stream)) return nullptr;

	wil::com_ptr_t<IWICBitmap> bitmap;
	HRESULT hr = istream_to_bitmap(stream.get(), bitmap);
	if FAILED(hr) hr = libwebp_istream_to_bitmap(stream.get(), bitmap);
	if FAILED(hr) return nullptr;
	if FAILED(fit_to(max_size, bitmap)) return nullptr;
	return new ComObject<JSImage>(bitmap, path);
}

IJSImage* Img::svg_to_image([[maybe_unused]] wil::zwstring_view path_or_xml, [[maybe_unused]] float max_width)
{
#ifdef NDEBUG
	std::string xml;
	if (path_or_xml.contains(L'<'))
	{
		xml = js::from_wide(path_or_xml);
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
			width = js::to_uint(svg_size.width);
			height = js::to_uint(svg_size.height);
		}
		else
		{
			ratio = max_width / svg_size.width;
			width = js::to_uint(max_width);
			height = js::to_uint(svg_size.height / svg_size.width * max_width);
		}

		const auto transform = resvg_transform(ratio, 0.f, 0.f, ratio, 0.f, 0.f);
		const auto stride = width * 4U;
		const auto size = stride * height;
		std::vector<uint8_t> data(size);
		wil::com_ptr_t<IWICBitmap> bitmap;

		resvg_render(tree, transform, width, height, reinterpret_cast<char*>(data.data()));
		resvg_tree_destroy(tree);
		tree = nullptr;

		if SUCCEEDED(factory::imaging->CreateBitmapFromMemory(width, height, GUID_WICPixelFormat32bppPRGBA, stride, size, data.data(), &bitmap))
		{
			return new ComObject<JSImage>(bitmap);
		}
	}
#endif
	return nullptr;
}

bool Img::save_as_jpg(IWICBitmap* bitmap, wil::zwstring_view path)
{
	album_art_data_ptr data;

	if FAILED(AlbumArtStatic::bitmap_to_jpg_data(bitmap, data))
	{
		return false;
	}

	return FileHelper(path).write(data->get_ptr(), data->get_size());
}

uint32_t Img::get_stream_size(IStream* stream)
{
	STATSTG stats{};
	if FAILED(stream->Stat(&stats, STATFLAG_DEFAULT)) return UINT_MAX;
	return stats.cbSize.LowPart;
}
