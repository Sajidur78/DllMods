#include "pch.h"
#include "ImGuiRenderDevice.h"
#include <vector>
#undef DrawText

namespace app::imgui
{
	using namespace csl::math;
	using namespace csl::ut;
	using namespace gindows;

	ImU32 ImGuiRenderDevice::CreateImGuiColor(const Color8& color)
	{
		return ImColor(color.r, color.g, color.b, color.a);
	}
	
	void ImGuiRenderDevice::DrawMouseCursor(const Point2<int>& point, const Color8& color,
	                                     MouseCursorType type)
	{
		printf("DrawMouseCursor\n");
	}

	void ImGuiRenderDevice::SetScreenBounds(const Rectangle2<int>& bound)
	{
		// Dumb hack to work around canvas setting the size to initialized desktop height
		auto boundCopy = bound;
		if (boundCopy.GetRight() == 1280 && boundCopy.GetBottom() == 720)
		{
			auto& io = ImGui::GetIO();
			boundCopy.width = static_cast<int>(io.DisplaySize.x);
			boundCopy.height = static_cast<int>(io.DisplaySize.y);
		}
		
		ImGui::GetOverlayDrawList()->PopClipRect();
		
		const ImVec2 min{ static_cast<float>(boundCopy.GetLeft()), static_cast<float>(boundCopy.GetTop()) };
		const ImVec2 max{ static_cast<float>(boundCopy.GetRight()), static_cast<float>(boundCopy.GetBottom()) };

		ImGui::GetOverlayDrawList()->PushClipRect(min, max, false);
		
		m_ScreenBoundary = bound;
	}

	void ImGuiRenderDevice::DrawIcon(const Point2<int>& point, const Color8& color,
		IconKind icon)
	{
		ImVec2 center{ (float)point.x + 4, (float)point.y + 4 };
		ImGui::GetOverlayDrawList()->AddCircleFilled(center, 4, CreateImGuiColor(color));
	}

	Size2<int> ImGuiRenderDevice::GetIconSize(IconKind kind) const
	{
		return Size2<int>{8, 8};
	}

	void ImGuiRenderDevice::DrawBorderFixed3D(const Rectangle2<int>& rect, const Color8& color,
		int width)
	{
		ImVec2 min{ (float)rect.x, (float)rect.y };
		ImVec2 max{ (float)(rect.x + rect.width), (float)(rect.y + rect.height) };
		
		ImGui::GetOverlayDrawList()->AddRect(min, max, CreateImGuiColor(color), 0, ImDrawCornerFlags_All, width);
	}

	void ImGuiRenderDevice::DrawRectangle(const Rectangle2<int>& rect, Color8& color)
	{
		ImVec2 min{ (float)rect.x, (float)rect.y };
		ImVec2 max{ (float)(rect.x + rect.width), (float)(rect.y + rect.height) };
		
		ImGui::GetOverlayDrawList()->AddRectFilled(min, max, CreateImGuiColor(color));
	}

	void ImGuiRenderDevice::DrawRectangle(const Rectangle2<int>& rect, Color8& color1,
		Color8& color2, Color8& color3, Color8& color4)
	{
		ImVec2 min{ (float)rect.GetLeft(), (float)rect.GetTop() };
		ImVec2 max{ (float)rect.GetRight(), (float)rect.GetBottom() };

		
		ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(min, max, CreateImGuiColor(color1), CreateImGuiColor(color4), 
			CreateImGuiColor(color2), CreateImGuiColor(color3));
	}

	void ImGuiRenderDevice::DrawLine(const Point2<int>& from, const Point2<int>& to,
		const Color8& color)
	{
		ImVec2 p1{ (float)from.x, (float)from.y };
		ImVec2 p2{ (float)to.x, (float)to.y };

		ImGui::GetOverlayDrawList()->AddLine(p1, p2, CreateImGuiColor(color));
	}

	void ImGuiRenderDevice::DrawString(void* a1, const Font* font, const Rectangle2<int>& rect,
		const Point2<int>& p1, const Point2<int>& p2, const Color8& color, const char* text)
	{
		const Vector2 pos{ static_cast<float>(p1.x + p2.x), static_cast<float>(p1.y + p2.y) };
		const auto size = font->GetSize();
		const Vector2 sizeVec = Vector2{static_cast<float>(size.width), static_cast<float>(size.height)};

		m_Font->SetTextColor(CreateImGuiColor(color));
		m_Font->SetTextSize(sizeVec);
		m_Font->DrawText(pos, text);
	}

	void ImGuiRenderDevice::DrawPrimitive(PrimType type, const PrimitiveVertex* a2, int a3)
	{
		printf("DrawPrimitive\n");
	}
}
