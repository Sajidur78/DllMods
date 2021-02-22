#pragma once

namespace app::imgui
{
	class ImGuiMouse : public gindows::device::Mouse
	{
		uint m_MouseType{};
		
	public:
		void Draw() override
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGui::RenderMouseCursor(ImGui::GetOverlayDrawList(), io.MousePos, 1, m_MouseType, static_cast<uint>(-1), 
				ImGui::GetColorU32(ImGuiCol_Border), ImGui::GetColorU32(ImGuiCol_BorderShadow));
		}

		void SetLocation(const csl::ut::Point2<int>& location) override
		{
			
		}

		[[nodiscard]] csl::ut::Point2<int> GetLocation() const override
		{
			ImGuiIO& io = ImGui::GetIO();

			return csl::ut::Point2<int> {static_cast<int>(io.MousePos.x), static_cast<int>(io.MousePos.y)};
		}

		void SetCursorType(gindows::MouseCursorType type) override
		{
			/*ImGuiIO& io = ImGui::GetIO();
			ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);*/

			switch(type)
			{
			case 0:
				m_MouseType = ImGuiMouseCursor_Arrow;
				break;

			case 2:
				m_MouseType = ImGuiMouseCursor_Hand;
				break;
				
			default:
				printf("Cursor type %d\n", type);
				break;
			}
		}
	};
}