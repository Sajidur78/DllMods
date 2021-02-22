#pragma once

namespace app::imgui
{
	class ImGuiDeviceFont : public gindows::device::Font
	{
	public:
		ImGuiDeviceFont(gindows::device::RenderState* pRenderState) : Font(pRenderState)
		{
			
		}

		[[nodiscard]] csl::ut::Size2<int> GetSize() const override
		{
			return csl::ut::Size2<int>{6, 12};
		}
	};
}
