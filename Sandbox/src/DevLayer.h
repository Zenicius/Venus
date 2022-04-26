#pragma once

#include "Venus.h"

class DevLayer : public Venus::Layer
{
	public:
		DevLayer();
		virtual ~DevLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		void OnUpdate(Venus::Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Venus::Event& e) override;
};

