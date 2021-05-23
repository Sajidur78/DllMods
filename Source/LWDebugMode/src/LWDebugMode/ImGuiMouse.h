#pragma once

namespace app::imgui
{
	class ImGuiMouse : public gindows::device::Mouse
	{
		uint m_MouseType{};
		csl::ut::Point2<int> m_MousePos{0, 0};
		
	public:
		void Draw() override;
		void SetCursorType(gindows::MouseCursorType type) override;
		void SetLocation(const csl::ut::Point2<int>& location) override
		{
			m_MousePos = location;
		}

		[[nodiscard]] csl::ut::Point2<int> GetLocation() const override
		{
			return m_MousePos;
		}
	};
}
