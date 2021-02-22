#pragma once

using namespace std;

namespace app::imgui
{
	class Window : public fnd::ReferencedObject
	{
		friend class WindowManager;
		
	protected:
		string m_title;
		bool m_isVisible{ true };
		ImVec2 m_position{};
		ImGuiWindowFlags m_windowFlags{};
		
	public:
		virtual void Render()
		{
			
		}

		[[nodiscard]] const string& GetTitle() const
		{
			return m_title;
		}

		[[nodiscard]] ImGuiWindowFlags GetWindowFlags() const
		{
			return m_windowFlags;
		}

		[[nodiscard]] const ImVec2& GetPosition() const
		{
			return m_position;
		}

		[[nodiscard]] bool IsVisible() const
		{
			return m_isVisible;
		}

		void SetVisibility(bool visible)
		{
			m_isVisible = visible;
		}

		void* operator new(size_t size);
	};
}