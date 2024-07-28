#include "stdafx.hpp"
#include "Gradient.hpp"

#pragma region static
HRESULT Gradient::CheckTwoNumberArray(JSON& j)
{
	if (j.is_array() && j.size() == 2 && j[0].is_number() && j[1].is_number()) return S_OK;
	return E_INVALIDARG;
}

JSON Gradient::Parse(std::wstring_view str)
{
	auto j = js::json_parse(str);

	if (j.is_object())
	{
		auto& jstops = j["Stops"];
		if (jstops.is_array() && jstops.size() >= 2)
		{
			return j;
		}
	}

	return JSON();
}

std::optional<D2D1_POINT_2F> Gradient::ToPoint(JSON& j)
{
	if FAILED(CheckTwoNumberArray(j)) return std::nullopt;

	const auto x = j[0].get<float>();
	const auto y = j[1].get<float>();
	return D2D1::Point2F(x, y);
}
#pragma endregion

HRESULT Gradient::CreateGradientStopCollection(JSON& jstops, wil::com_ptr_t<ID2D1GradientStopCollection>& collection)
{
	std::vector<D2D1_GRADIENT_STOP> stops;

	for (auto&& jstop : jstops)
	{
		RETURN_IF_FAILED(CheckTwoNumberArray(jstop));

		const auto pos = jstop[0].get<float>();
		const auto colour = js::to_colorf(jstop[1]);
		const auto stop = D2D1::GradientStop(pos, colour);
		stops.emplace_back(stop);
	}

	return m_context->CreateGradientStopCollection(stops.data(), js::sizeu(stops), &collection);
}

HRESULT Gradient::CreateLinearBrush(JSON& obj, float x, float y)
{
	auto start = ToPoint(obj["Start"]);
	auto end = ToPoint(obj["End"]);

	if (start && end)
	{
		auto& jstops = obj["Stops"];
		const auto stop_string = jstops.dump();
		if (!m_linear_brush || stop_string != m_linear_stop_string)
		{
			D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES properties{};
			wil::com_ptr_t<ID2D1GradientStopCollection> collection;

			RETURN_IF_FAILED(CreateGradientStopCollection(jstops, collection));
			RETURN_IF_FAILED(m_context->CreateLinearGradientBrush(properties, collection.get(), &m_linear_brush));
			m_linear_stop_string = stop_string;
		}

		const auto sp = D2D1::Point2F(x + start->x, y + start->y);
		const auto ep = D2D1::Point2F(x + end->x, y + end->y);
		m_linear_brush->SetStartPoint(sp);
		m_linear_brush->SetEndPoint(ep);
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT Gradient::CreateRadialBrush(JSON& obj, float x, float y)
{
	auto centre = ToPoint(obj["Centre"]);
	auto radius = ToPoint(obj["Radius"]);

	if (centre && radius)
	{
		auto& jstops = obj["Stops"];
		const auto stop_string = jstops.dump();
		if (!m_radial_brush || stop_string != m_radial_stop_string)
		{
			D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES properties{};
			wil::com_ptr_t<ID2D1GradientStopCollection> collection;

			RETURN_IF_FAILED(CreateGradientStopCollection(jstops, collection));
			RETURN_IF_FAILED(m_context->CreateRadialGradientBrush(properties, collection.get(), &m_radial_brush));
			m_radial_stop_string = stop_string;
		}

		m_radial_brush->SetCenter(D2D1::Point2F(x + centre->x, y + centre->y));
		m_radial_brush->SetRadiusX(radius->x);
		m_radial_brush->SetRadiusY(radius->y);
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT Gradient::DrawEllipse(const D2D1_ELLIPSE& ellipse, float line_width, std::wstring_view str)
{
	auto j = Parse(str);
	RETURN_HR_IF(E_INVALIDARG, j.is_null());

	const auto x = ellipse.point.x - ellipse.radiusX;
	const auto y = ellipse.point.y - ellipse.radiusY;

	if SUCCEEDED(CreateLinearBrush(j, x, y))
	{
		m_context->DrawEllipse(ellipse, m_linear_brush.get(), line_width);
		return S_OK;
	}

	if SUCCEEDED(CreateRadialBrush(j, x, y))
	{
		m_context->DrawEllipse(ellipse, m_radial_brush.get(), line_width);
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT Gradient::DrawLine(const D2D1_POINT_2F& p1, const D2D1_POINT_2F& p2, float line_width, std::wstring_view str)
{
	auto j = Parse(str);
	RETURN_HR_IF(E_INVALIDARG, j.is_null());

	const auto x = p1.x;
	const auto y = p1.y;

	if SUCCEEDED(CreateLinearBrush(j, x, y))
	{
		m_context->DrawLine(p1, p2, m_linear_brush.get(), line_width);
		return S_OK;
	}

	if SUCCEEDED(CreateRadialBrush(j, x, y))
	{
		m_context->DrawLine(p1, p2, m_radial_brush.get(), line_width);
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT Gradient::DrawRectangle(const D2D1_RECT_F& rect, float line_width, std::wstring_view str)
{
	auto j = Parse(str);
	RETURN_HR_IF(E_INVALIDARG, j.is_null());

	const auto x = rect.left;
	const auto y = rect.top;

	if SUCCEEDED(CreateLinearBrush(j, x, y))
	{
		m_context->DrawRectangle(rect, m_linear_brush.get(), line_width);
		return S_OK;
	}

	if SUCCEEDED(CreateRadialBrush(j, x, y))
	{
		m_context->DrawRectangle(rect, m_radial_brush.get(), line_width);
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT Gradient::DrawRoundedRectangle(const D2D1_ROUNDED_RECT& rounded_rect, float line_width, std::wstring_view str)
{
	auto j = Parse(str);
	RETURN_HR_IF(E_INVALIDARG, j.is_null());

	const auto x = rounded_rect.rect.left;
	const auto y = rounded_rect.rect.top;

	if SUCCEEDED(CreateLinearBrush(j, x, y))
	{
		m_context->DrawRoundedRectangle(rounded_rect, m_linear_brush.get(), line_width);
		return S_OK;
	}

	if SUCCEEDED(CreateRadialBrush(j, x, y))
	{
		m_context->DrawRoundedRectangle(rounded_rect, m_radial_brush.get(), line_width);
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT Gradient::FillEllipse(const D2D1_ELLIPSE& ellipse, std::wstring_view str)
{
	auto j = Parse(str);
	RETURN_HR_IF(E_INVALIDARG, j.is_null());

	const auto x = ellipse.point.x - ellipse.radiusX;
	const auto y = ellipse.point.y - ellipse.radiusY;

	if SUCCEEDED(CreateLinearBrush(j, x, y))
	{
		m_context->FillEllipse(ellipse, m_linear_brush.get());
		return S_OK;
	}

	if SUCCEEDED(CreateRadialBrush(j, x, y))
	{
		m_context->FillEllipse(ellipse, m_radial_brush.get());
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT Gradient::FillRectangle(const D2D1_RECT_F& rect, std::wstring_view str)
{
	auto j = Parse(str);
	RETURN_HR_IF(E_INVALIDARG, j.is_null());

	const auto x = rect.left;
	const auto y = rect.top;

	if SUCCEEDED(CreateLinearBrush(j, x, y))
	{
		m_context->FillRectangle(rect, m_linear_brush.get());
		return S_OK;
	}

	if SUCCEEDED(CreateRadialBrush(j, x, y))
	{
		m_context->FillRectangle(rect, m_radial_brush.get());
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT Gradient::FillRoundedRectangle(const D2D1_ROUNDED_RECT& rounded_rect, std::wstring_view str)
{
	auto j = Parse(str);
	RETURN_HR_IF(E_INVALIDARG, j.is_null());

	const auto x = rounded_rect.rect.left;
	const auto y = rounded_rect.rect.top;

	if SUCCEEDED(CreateLinearBrush(j, x, y))
	{
		m_context->FillRoundedRectangle(rounded_rect, m_linear_brush.get());
		return S_OK;
	}

	if SUCCEEDED(CreateRadialBrush(j, x, y))
	{
		m_context->FillRoundedRectangle(rounded_rect, m_radial_brush.get());
		return S_OK;
	}

	return E_INVALIDARG;
}

void Gradient::Init(ID2D1DeviceContext* context)
{
	m_context = context;
}

void Gradient::Reset()
{
	m_linear_stop_string.clear();
	m_radial_stop_string.clear();

	m_linear_brush.reset();
	m_radial_brush.reset();
}
