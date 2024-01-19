#include "stdafx.hpp"
#include "IStreamHelpers.hpp"

HRESULT IStreamHelpers::check_size(IStream* stream)
{
	if (get_size(stream) > Component::max_image_size) return E_INVALIDARG;
	return S_OK;
}

HRESULT IStreamHelpers::create_from_album_art_data(const album_art_data_ptr& data, wil::com_ptr_t<IStream>& stream)
{
	RETURN_HR_IF(E_FAIL, data.is_empty());

	auto ptr = static_cast<const uint8_t*>(data->data());
	const uint32_t size = to_uint(data->size());

	auto tmp = SHCreateMemStream(ptr, size);
	RETURN_HR_IF_NULL(E_FAIL, tmp);

	stream.attach(tmp);
	return S_OK;
}

HRESULT IStreamHelpers::create_from_path(wil::zwstring_view path, wil::com_ptr_t<IStream>& stream)
{
	RETURN_IF_FAILED(SHCreateStreamOnFileEx(path.data(), STGM_READ | STGM_SHARE_DENY_WRITE, GENERIC_READ, FALSE, nullptr, &stream));
	RETURN_IF_FAILED(check_size(stream.get()));
	return S_OK;
}

album_art_data_ptr IStreamHelpers::to_album_art_data(IStream* stream)
{
	const auto size = get_size(stream);
	if (size <= Component::max_image_size)
	{
		auto data = fb2k::service_new<album_art_data_impl>();
		data->set_size(size);
		ULONG bytes_read{};
		if SUCCEEDED(stream->Read(data->get_ptr(), size, &bytes_read))
		{
			return data;
		}
	}
	return album_art_data_ptr();
}

uint32_t IStreamHelpers::get_size(IStream* stream)
{
	STATSTG stats{};
	if FAILED(stream->Stat(&stats, STATFLAG_DEFAULT)) return UINT_MAX;
	return stats.cbSize.LowPart;
}
