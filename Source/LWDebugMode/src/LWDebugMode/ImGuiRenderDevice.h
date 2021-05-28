#pragma once
#include "ImGuiFont.h"

namespace app::imgui
{
	class ImGuiRenderDevice : public gindows::device::Graphics
	{
	private:
		csl::ut::Rectangle2<int> m_ScreenBoundary{ 0, 0, 0, 0 };
		static ImU32 CreateImGuiColor(const csl::ut::Color8& color);
		font::Font* m_Font{};
		
	public:
		ImGuiRenderDevice() : Graphics(nullptr)
		{
			m_Font = csl::fnd::Singleton<font::FontManager>::GetInstance()->GetDbgFont();
		}

		void DrawMouseCursor(const csl::ut::Point2<int>& point, const csl::ut::Color8& color, gindows::MouseCursorType type) override;
		void SetScreenBounds(const csl::ut::Rectangle2<int>& bound) override final;
		void DrawIcon(const csl::ut::Point2<int>& point, const csl::ut::Color8& color, gindows::IconKind icon) override;
		[[nodiscard]] csl::ut::Size2<int> GetIconSize(gindows::IconKind kind) const override;
		void DrawBorderFixed3D(const csl::ut::Rectangle2<int>& rect, const csl::ut::Color8& color, int a3) override;
		void DrawRectangle(const csl::ut::Rectangle2<int>& rect, csl::ut::Color8& color) override;
		void DrawRectangle(const csl::ut::Rectangle2<int>& rect, csl::ut::Color8& color1, csl::ut::Color8& color2, csl::ut::Color8& color3, csl::ut::Color8& color4) override;
		void DrawLine(const csl::ut::Point2<int>& from, const csl::ut::Point2<int>& to, const csl::ut::Color8& color) override;
		csl::ut::Point2<int> DrawString(const gindows::device::Font* font, const csl::ut::Rectangle2<int>& rect, const csl::ut::Point2<int>& p1, const csl::ut::Point2<int>& p2, const csl::ut::Color8& color, const char* text) override;
		void DrawPrimitive(gindows::PrimType type, const gindows::PrimitiveVertex* a2, int a3) override;

		void FlushVertex() override
		{
			
		}

		void BeginDraw() override
		{
			m_Font->Begin();
		}

		void EndDraw() override
		{
			m_Font->End();
		}
	};
}
