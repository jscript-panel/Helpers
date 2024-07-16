#pragma once

class Gradient
{
public:
	HRESULT DrawEllipse(const D2D1_ELLIPSE& ellipse, float line_width, std::wstring_view str);
	HRESULT DrawLine(const D2D1_POINT_2F& p1, const D2D1_POINT_2F& p2, float line_width, std::wstring_view str);
	HRESULT DrawRectangle(const D2D1_RECT_F& rect, float line_width, std::wstring_view str);
	HRESULT DrawRoundedRectangle(const D2D1_ROUNDED_RECT& rounded_rect, float line_width, std::wstring_view str);
	HRESULT FillEllipse(const D2D1_ELLIPSE& ellipse, std::wstring_view str);
	HRESULT FillRectangle(const D2D1_RECT_F& rect, std::wstring_view str);
	HRESULT FillRoundedRectangle(const D2D1_ROUNDED_RECT& rounded_rect, std::wstring_view str);
	void Init(ID2D1DeviceContext* context);
	void Reset();

private:
	static HRESULT CheckTwoNumberArray(JSON& j);
	static JSON Parse(std::wstring_view str);
	static std::optional<D2D1_POINT_2F> ToPoint(JSON& j);

	HRESULT CreateGradientStopCollection(JSON& jstops, wil::com_ptr_t<ID2D1GradientStopCollection>& collection);
	HRESULT CreateLinearBrush(JSON& obj, float x, float y);
	HRESULT CreateRadialBrush(JSON& obj, float x, float y);

	ID2D1DeviceContext* m_context;
	std::string m_linear_stop_string, m_radial_stop_string;
	wil::com_ptr_t<ID2D1LinearGradientBrush> m_linear_brush;
	wil::com_ptr_t<ID2D1RadialGradientBrush> m_radial_brush;
};
