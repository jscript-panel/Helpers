#pragma once

class LayerScope
{
public:
	LayerScope(ID2D1DeviceContext* context, float x, float y, float w, float h) : m_context(context)
	{
		const auto rect = to_rectf(x, y, w, h);
		const auto layer_params = D2D1::LayerParameters(rect);
		wil::com_ptr_t<ID2D1Layer> layer;

		m_context->CreateLayer(&layer);
		m_context->PushLayer(layer_params, layer.get());
	}

	~LayerScope()
	{
		m_context->PopLayer();
	}

private:
	ID2D1DeviceContext* m_context{};
};
