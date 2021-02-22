#include "pch.h"
#include "ImGuiFont.h"

#pragma push_macro("DrawText")
#undef DrawText

namespace app::font
{
	const char* ImGuiFont::ShiftJisToUtf8(const char* value) {
		WCHAR wideChar[1024];

		MultiByteToWideChar(932, 0, value, -1, wideChar, 1024);
		WideCharToMultiByte(CP_UTF8, 0, wideChar, -1, ms_utf8Buf, 1024, 0, 0);
		return ms_utf8Buf;
	}

	void ImGuiFont::SetTextColor(uint color)
	{
		m_TextColor = color;
	}

	void ImGuiFont::SetTextSize(const csl::math::Vector2& size)
	{
		m_TextSize = size.GetX();

		if (size.GetY() > size.GetX())
			m_TextSize = sqrtf((size.x * size.x) + (size.y * size.y));
	}

	Vector2* ImGuiFont::GetFontSize(csl::math::Vector2* pSize)
	{
		if (!pSize)
			pSize = new Vector2();

		*pSize = Vector2{ m_TextSize, m_TextSize };
		
		return pSize;
	}

	void ImGuiFont::DrawText(const csl::math::Vector2& pos, const char* text)
	{
		const ImVec2 imPos = ImVec2{ pos.GetX(), pos.GetY() };
		
		ImGui::GetOverlayDrawList()->AddText(m_pFont, m_TextSize, imPos, m_TextColor, ShiftJisToUtf8(text));
	}

	void ImGuiFont::DrawText(const csl::math::Vector2& pos, const wchar_t* text)
	{
		std::wstring wStr = text;
		const std::string str = std::string(wStr.begin(), wStr.end());

		DrawText(pos, str.c_str());
	}

	Vector2* ImGuiFont::GetCharSize(wchar_t ch, csl::math::Vector2* pSize)
	{
		if (!pSize)
			pSize = new Vector2();

		*pSize = Vector2{ 0, 0 };

		return pSize;
	}

	const wchar_t* ImGuiFont::ToWideString(const char* text)
	{
		std::string str = text;
		return std::wstring(str.begin(), str.end()).c_str();
	}
}

#pragma pop_macro("DrawText")