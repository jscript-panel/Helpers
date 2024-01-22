#pragma once

class GdiScope
{
public:
	GdiScope(ID2D1DeviceContext* context)
	{
		if FAILED(context->QueryInterface(IID_PPV_ARGS(&gdi))) return;
		if FAILED(gdi->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &dc)) return;
		is_valid = true;
	}

	~GdiScope()
	{
		if (is_valid) gdi->ReleaseDC(nullptr);
	}

	operator bool() const { return is_valid; }

	HDC dc{};

private:
	bool is_valid{};
	wil::com_ptr_t<ID2D1GdiInteropRenderTarget> gdi;
};
