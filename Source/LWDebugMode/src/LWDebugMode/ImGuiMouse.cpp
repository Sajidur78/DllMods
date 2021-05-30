#include "ImGuiMouse.h"
#include <imgui_internal.h>

#include "Mod.h"

namespace app::imgui
{
	void ImGuiMouse::Draw()
	{
		if (!Mod::GetInstance()->GetConfig().m_SoftwareCursor)
			return;
		
		ImGui::RenderMouseCursor(ImGui::GetOverlayDrawList(), ImVec2{ static_cast<float>(m_MousePos.x), static_cast<float>(m_MousePos.y) }, 1, m_MouseType, static_cast<uint>(-1),
			ImGui::GetColorU32(ImGuiCol_Border), ImGui::GetColorU32(ImGuiCol_BorderShadow));
	}

	void ImGuiMouse::SetCursorType(gindows::MouseCursorType type)
	{
		switch (type)
		{
		case 0:
		case 2:
			m_MouseType = ImGuiMouseCursor_Arrow;
			break;

		case 4:
			m_MouseType = ImGuiMouseCursor_ResizeNS;
			break;

		case 5:
			m_MouseType = ImGuiMouseCursor_ResizeEW;
			break;

		case 6:
			m_MouseType = ImGuiMouseCursor_ResizeNWSE;
			break;

		case 7:
			m_MouseType = ImGuiMouseCursor_ResizeNESW;
			break;

		default:
			printf("Cursor type %d\n", type);
			break;
		}
	}
}
