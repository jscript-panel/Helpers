#pragma once

class IStreamHelpers
{
public:
	static HRESULT check_size(IStream* stream);
	static HRESULT create_from_album_art_data(const album_art_data_ptr& data, wil::com_ptr_t<IStream>& stream);
	static HRESULT create_from_path(wil::zwstring_view path, wil::com_ptr_t<IStream>& stream);
	static album_art_data_ptr to_album_art_data(IStream* stream);
	static uint32_t get_size(IStream* stream);
};
