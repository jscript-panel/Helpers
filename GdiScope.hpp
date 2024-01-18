#pragma once

struct GdiScope
{
	GdiScope(ID2D1RenderTarget* render_target)
	{
		if SUCCEEDED(render_target->QueryInterface(IID_PPV_ARGS(&gdi)))
		{
			gdi->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &dc);
		}
	}

	~GdiScope()
	{
		if (gdi) gdi->ReleaseDC(nullptr);
	}

	operator bool() const { return dc != nullptr; }

	HDC dc{};
	wil::com_ptr_t<ID2D1GdiInteropRenderTarget> gdi;
};
