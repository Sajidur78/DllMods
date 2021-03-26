#pragma once
#pragma push_macro("DrawText")
#undef DrawText

namespace app::font
{
	using namespace csl::math;
	class ImGuiFont : public Font
	{
		inline static char ms_utf8Buf[1024];
		ImFont* m_pFont{};
		ImU32 m_TextColor{};
		float m_TextSize{ 5 };
		
		static const char* ShiftJisToUtf8(const char* value);

	public:
		bool IsInit() override
		{
			return ImGui::GetCurrentContext() != nullptr;
		}

		void SetTextColor(uint color) override;
		void SetTextSize(const csl::math::Vector2& size) override;
		Vector2* GetFontSize(csl::math::Vector2* pSize) override;

		void DrawText(const Vector2& pos, const char* text) override;
		void DrawText(const Vector2& pos, const wchar_t* text) override;

		void DrawText(const Vector2& pos, char* text) override
		{
			DrawText(pos, static_cast<const char*>(text));
		}

		void DrawText(const Vector2& pos, wchar_t* text) override
		{
			DrawText(pos, static_cast<const wchar_t*>(text));
		}

		Vector2* GetCharSize(wchar_t ch, csl::math::Vector2* pSize) override;
		const wchar_t* ToWideString(const char* text) override;

		void SetFont(ImFont* pFont)
		{
			m_pFont = pFont;
		}
	};
}

#pragma pop_macro("DrawText")