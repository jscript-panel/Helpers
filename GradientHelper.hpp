#pragma once

class GradientHelper
{
public:
	HRESULT DrawEllipse(const D2D1_ELLIPSE& ellipse, float line_width, wil::zwstring_view str);
	HRESULT DrawLine(const D2D1_POINT_2F& p1, const D2D1_POINT_2F& p2, float line_width, wil::zwstring_view str);
	HRESULT DrawRectangle(const D2D1_RECT_F& rect, float line_width, wil::zwstring_view str);
	HRESULT DrawRoundedRectangle(const D2D1_ROUNDED_RECT& rounded_rect, float line_width, wil::zwstring_view str);
	HRESULT FillEllipse(const D2D1_ELLIPSE& ellipse, wil::zwstring_view str);
	HRESULT FillRectangle(const D2D1_RECT_F& rect, wil::zwstring_view str);
	HRESULT FillRoundedRectangle(const D2D1_ROUNDED_RECT& rounded_rect, wil::zwstring_view str);
	void Init(ID2D1DeviceContext* context);

private:
	static HRESULT CreateLinearProperties(JSON& obj, float x, float y, D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES& properties);
	static HRESULT CreateRadialProperties(JSON& obj, float x, float y, D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES& properties);

	HRESULT CreateGradientStopCollection(JSON& jstops);
	HRESULT CreateLinearBrush(JSON& jstops, const D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES& properties);
	HRESULT CreateRadialBrush(JSON& jstops, const D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES& properties);

	ID2D1DeviceContext* m_context;
	wil::com_ptr_t<ID2D1GradientStopCollection> m_collection;
	wil::com_ptr_t<ID2D1LinearGradientBrush> m_linear_brush;
	wil::com_ptr_t<ID2D1RadialGradientBrush> m_radial_brush;
};
