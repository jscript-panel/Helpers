#include "stdafx.hpp"
#include "GradientHelper.hpp"

HRESULT GradientHelper::CreateGradientStopCollection(JSON& jstops)
{
	if (jstops.is_array() && jstops.size() >= 2)
	{
		std::vector<D2D1_GRADIENT_STOP> stops;

		for (auto&& jstop : jstops)
		{
			RETURN_IF_FAILED(JSONHelper::check_two_number_array(jstop));

			const auto pos = jstop[0].get<float>();
			const auto colour = jstop[1].get<int64_t>();
			const auto stop = D2D1::GradientStop(pos, to_colorf(colour));
			stops.emplace_back(stop);
		}

		return m_context->CreateGradientStopCollection(stops.data(), to_uint(stops.size()), &m_collection);
	}
	return E_INVALIDARG;
}

HRESULT GradientHelper::CreateLinearBrush(JSON& jstops, const D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES& properties)
{
	RETURN_IF_FAILED(CreateGradientStopCollection(jstops));
	RETURN_IF_FAILED(m_context->CreateLinearGradientBrush(properties, m_collection.get(), &m_linear_brush));
	return S_OK;
}

HRESULT GradientHelper::CreateLinearProperties(JSON& obj, float x, float y, D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES& properties)
{
	auto start = JSONHelper::to_point(obj["Start"]);
	auto end = JSONHelper::to_point(obj["End"]);

	if (start && end)
	{
		properties.startPoint = D2D1::Point2F(x + start->x, y + start->y);
		properties.endPoint = D2D1::Point2F(x + end->x, y + end->y);
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT GradientHelper::CreateRadialBrush(JSON& jstops, const D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES& properties)
{
	RETURN_IF_FAILED(CreateGradientStopCollection(jstops));
	RETURN_IF_FAILED(m_context->CreateRadialGradientBrush(properties, m_collection.get(), &m_radial_brush));
	return S_OK;
}

HRESULT GradientHelper::CreateRadialProperties(JSON& obj, float x, float y, D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES& properties)
{
	auto centre = JSONHelper::to_point(obj["Centre"]);
	auto radius = JSONHelper::to_point(obj["Radius"]);

	if (centre && radius)
	{
		properties.center = D2D1::Point2F(x + centre->x, y + centre->y);
		properties.radiusX = radius->x;
		properties.radiusY = radius->y;
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT GradientHelper::DrawEllipse(const D2D1_ELLIPSE& ellipse, float line_width, wil::zwstring_view str)
{
	auto j = JSONHelper::parse(str);
	if (!j.is_object()) return E_INVALIDARG;

	const auto x = ellipse.point.x - ellipse.radiusX;
	const auto y = ellipse.point.y - ellipse.radiusY;

	D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES lp{};
	if SUCCEEDED(CreateLinearProperties(j, x, y, lp))
	{
		RETURN_IF_FAILED(CreateLinearBrush(j["Stops"], lp));
		m_context->DrawEllipse(ellipse, m_linear_brush.get(), line_width);
		return S_OK;
	}

	D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES rp{};
	if SUCCEEDED(CreateRadialProperties(j, x, y, rp))
	{
		RETURN_IF_FAILED(CreateRadialBrush(j["Stops"], rp));
		m_context->DrawEllipse(ellipse, m_radial_brush.get(), line_width);
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT GradientHelper::DrawLine(const D2D1_POINT_2F& p1, const D2D1_POINT_2F& p2, float line_width, wil::zwstring_view str)
{
	auto j = JSONHelper::parse(str);
	if (!j.is_object()) return E_INVALIDARG;

	D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES lp{};
	if SUCCEEDED(CreateLinearProperties(j, p1.x, p1.y, lp))
	{
		RETURN_IF_FAILED(CreateLinearBrush(j["Stops"], lp));
		m_context->DrawLine(p1, p2, m_linear_brush.get(), line_width);
		return S_OK;
	}

	D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES rp{};
	if SUCCEEDED(CreateRadialProperties(j, p1.x, p1.y, rp))
	{
		RETURN_IF_FAILED(CreateRadialBrush(j["Stops"], rp));
		m_context->DrawLine(p1, p2, m_radial_brush.get(), line_width);
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT GradientHelper::DrawRectangle(const D2D1_RECT_F& rect, float line_width, wil::zwstring_view str)
{
	auto j = JSONHelper::parse(str);
	if (!j.is_object()) return E_INVALIDARG;

	D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES lp{};
	if SUCCEEDED(CreateLinearProperties(j, rect.left, rect.top, lp))
	{
		RETURN_IF_FAILED(CreateLinearBrush(j["Stops"], lp));
		m_context->DrawRectangle(rect, m_linear_brush.get(), line_width);
		return S_OK;
	}

	D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES rp{};
	if SUCCEEDED(CreateRadialProperties(j, rect.left, rect.top, rp))
	{
		RETURN_IF_FAILED(CreateRadialBrush(j["Stops"], rp));
		m_context->DrawRectangle(rect, m_radial_brush.get(), line_width);
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT GradientHelper::DrawRoundedRectangle(const D2D1_ROUNDED_RECT& rounded_rect, float line_width, wil::zwstring_view str)
{
	auto j = JSONHelper::parse(str);
	if (!j.is_object()) return E_INVALIDARG;

	D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES lp{};
	if SUCCEEDED(CreateLinearProperties(j, rounded_rect.rect.left, rounded_rect.rect.top, lp))
	{
		RETURN_IF_FAILED(CreateLinearBrush(j["Stops"], lp));
		m_context->DrawRoundedRectangle(rounded_rect, m_linear_brush.get(), line_width);
		return S_OK;
	}

	D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES rp{};
	if SUCCEEDED(CreateRadialProperties(j, rounded_rect.rect.left, rounded_rect.rect.top, rp))
	{
		RETURN_IF_FAILED(CreateRadialBrush(j["Stops"], rp));
		m_context->DrawRoundedRectangle(rounded_rect, m_radial_brush.get(), line_width);
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT GradientHelper::FillEllipse(const D2D1_ELLIPSE& ellipse, wil::zwstring_view str)
{
	auto j = JSONHelper::parse(str);
	if (!j.is_object()) return E_INVALIDARG;

	const auto x = ellipse.point.x - ellipse.radiusX;
	const auto y = ellipse.point.y - ellipse.radiusY;

	D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES lp{};
	if SUCCEEDED(CreateLinearProperties(j, x, y, lp))
	{
		RETURN_IF_FAILED(CreateLinearBrush(j["Stops"], lp));
		m_context->FillEllipse(ellipse, m_linear_brush.get());
		return S_OK;
	}

	D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES rp{};
	if SUCCEEDED(CreateRadialProperties(j, x, y, rp))
	{
		RETURN_IF_FAILED(CreateRadialBrush(j["Stops"], rp));
		m_context->FillEllipse(ellipse, m_radial_brush.get());
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT GradientHelper::FillRectangle(const D2D1_RECT_F& rect, wil::zwstring_view str)
{
	auto j = JSONHelper::parse(str);
	if (!j.is_object()) return E_INVALIDARG;

	D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES lp{};
	if SUCCEEDED(CreateLinearProperties(j, rect.left, rect.top, lp))
	{
		RETURN_IF_FAILED(CreateLinearBrush(j["Stops"], lp));
		m_context->FillRectangle(rect, m_linear_brush.get());
		return S_OK;
	}

	D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES rp{};
	if SUCCEEDED(CreateRadialProperties(j, rect.left, rect.top, rp))
	{
		RETURN_IF_FAILED(CreateRadialBrush(j["Stops"], rp));
		m_context->FillRectangle(rect, m_radial_brush.get());
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT GradientHelper::FillRoundedRectangle(const D2D1_ROUNDED_RECT& rounded_rect, wil::zwstring_view str)
{
	auto j = JSONHelper::parse(str);
	if (!j.is_object()) return E_INVALIDARG;

	D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES lp{};
	if SUCCEEDED(CreateLinearProperties(j, rounded_rect.rect.left, rounded_rect.rect.top, lp))
	{
		RETURN_IF_FAILED(CreateLinearBrush(j["Stops"], lp));
		m_context->FillRoundedRectangle(rounded_rect, m_linear_brush.get());
		return S_OK;
	}

	D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES rp{};
	if SUCCEEDED(CreateRadialProperties(j, rounded_rect.rect.left, rounded_rect.rect.top, rp))
	{
		RETURN_IF_FAILED(CreateRadialBrush(j["Stops"], rp));
		m_context->FillRoundedRectangle(rounded_rect, m_radial_brush.get());
		return S_OK;
	}

	return E_INVALIDARG;
}

void GradientHelper::Init(ID2D1DeviceContext* context)
{
	m_context = context;
}
