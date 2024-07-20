#pragma once

class LoadImageAsync : public SimpleThreadTask
{
public:
	LoadImageAsync(CWindow wnd, std::wstring_view path) : m_wnd(wnd), m_path(path) {}

	void run() final
	{
		IJSImage* image = js::path_to_image(m_path);
		auto data = ImageCallbackData(m_path, image);
		m_wnd.SendMessageW(std::to_underlying(CallbackID::on_load_image_done), reinterpret_cast<WPARAM>(&data));
	}

private:
	CWindow m_wnd;
	std::wstring m_path;
};
