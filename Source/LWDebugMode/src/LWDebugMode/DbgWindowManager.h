#pragma once
#include "ImGuiMouse.h"
#include <vector>

namespace app::dbg
{
	struct WindowInitNode
	{
		typedef gindows::Control* __cdecl WindowInit();
		inline static WindowInitNode* ms_pRootNode{};
		
		const char* m_pName{};
		WindowInit* m_pConstructor{};
		WindowInitNode* m_pPrevNode{};

		WindowInitNode(const char* pName, WindowInit& pInit) : m_pName(pName), m_pConstructor(&pInit)
		{
			m_pPrevNode = ms_pRootNode;
			ms_pRootNode = this;
		}
		
		static WindowInitNode* FindByName(const char* pName)
		{
			WindowInitNode* pCurNode = ms_pRootNode;
			while(pCurNode)
			{
				if (!strcmp(pCurNode->m_pName, pName))
					return pCurNode;

				pCurNode = pCurNode->m_pPrevNode;
			}
			
			return nullptr;
		}
	};
	
	class WindowManager : public fnd::ReferencedObject, csl::fnd::SingletonPointer<WindowManager>
	{
		inline static FUNCTION_PTR(gindows::Control*, __thiscall, ms_fpFindWindowByName, ASLR(0x004451E0), const WindowManager* pThis, const char* pName);
		inline static FUNCTION_PTR(void, __thiscall, gindowsControlSetSize, ASLR(0x0096FFD0), void* pControl, const csl::ut::Size2<int>& size);
		inline static FUNCTION_PTR(void, __thiscall, gindowsProcessMouseMove, ASLR(0x0096EF00), gindows::Manager* pManager, const csl::ut::Point2<int>& point);
		inline static FUNCTION_PTR(void, __thiscall, gindowsProcessMouseDown, ASLR(0x0096F180), gindows::Manager* pManager, uint kind);
		inline static FUNCTION_PTR(void, __thiscall, gindowsProcessMouseUp, ASLR(0x0096F190), gindows::Manager* pManager, uint kind);
		inline static FUNCTION_PTR(void, __thiscall, gindowsProcessMouseWheel, ASLR(0x0096F1A0), gindows::Manager* pManager, int delta);
		
		inline static FUNCTION_PTR(void, __thiscall, gindowsProcessKeyPress, ASLR(0x0096EB40), gindows::Manager* pManager, char key);
		inline static FUNCTION_PTR(void, __thiscall, gindowsProcessKeyDown, ASLR(0x0096EAA0), gindows::Manager* pManager, ushort key, bool a2, bool a3, bool a4);
		inline static FUNCTION_PTR(void, __thiscall, gindowsProcessKeyUp, ASLR(0x0096EAF0), gindows::Manager* pManager, ushort key, bool a2, bool a3, bool a4);
		inline static FUNCTION_PTR(char, __stdcall, AsciiFromKeycode, ASLR(0x009725F0), gindows::Key keyCode, bool caps);
		
	protected:
		csl::ut::MoveArray<gindows::Control*> m_Windows{ 32, GetAllocator() };
		csl::ut::MoveArray<void*> m_States{ 32, GetAllocator() };
		csl::ut::MoveArray<void*> m_Layouts{ 32, GetAllocator() };
		void* unk1{};
		
	public:
		WindowManager()
		{
			ReplaceInstance(this);
			gindows::Manager::GetInstance()->SetFont(0, new imgui::ImGuiDeviceFont(nullptr));
			gindows::Manager::GetInstance()->SetGraphics(new imgui::ImGuiRenderDevice());
			gindows::Manager::GetInstance()->SetMouse(new imgui::ImGuiMouse());

			auto* inst = gindows::Manager::GetInstance();
			auto* backColor = gindows::Manager::GetInstance()->GetDefaultBackColorPointer();
			backColor->a = 100;
			
			const csl::ut::Size2<int> size{ 1280, 720 };
			gindowsControlSetSize(gindows::Manager::GetInstance()->GetDesktopPointer(), size);
		}

		void AddWindow(gindows::Control* window)
		{
			m_Windows.push_back(window);
		}

		void RemoveWindow(gindows::Control* pWindow)
		{
			size_t idx = -1;
			for (size_t i = 0; i < m_Windows.size(); i++)
			{
				if (m_Windows[i] == pWindow)
				{
					idx = i;
					break;
				}
			}

			if (idx != -1)
				m_Windows.remove(idx);
		}
		
		void FormDestroyEventHandler(gindows::Object* pControl, gindows::EventArgs& args);
		gindows::Control* CreateWindowByName(const char* pName);

		gindows::Control* FindWindowByName(const char* pName) const
		{
			return ms_fpFindWindowByName(this, pName);
		}
		
		static void Render()
		{
			ImGuiIO& io = ImGui::GetIO();

			auto* fontMgr = csl::fnd::Singleton<font::FontManager>::GetInstance();
			const csl::ut::Size2<int> size{static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y)};
			gindowsControlSetSize(gindows::Manager::GetInstance()->GetDesktopPointer(), size);
			auto* menuMan = csl::fnd::Singleton<MenuManager>::GetInstance();
			
			if (menuMan && menuMan->GetSelectedMenu())
			{
				menuMan->GetSelectedMenu()->Draw(fontMgr->GetDbgFont());
			}

			fontMgr->DbgDraw();
			
			gindows::Manager::GetInstance()->Render();
		}

		static void MouseMove()
		{
			ImGuiIO& io = ImGui::GetIO();
			const csl::ut::Point2<int> mousePoint{ static_cast<int>(io.MousePos.x), static_cast<int>(io.MousePos.y) };

			gindowsProcessMouseMove(gindows::Manager::GetInstance(), mousePoint);
		}
		
		static void MouseDown(uint kind)
		{
			auto* instance = gindows::Manager::GetInstance();
			gindowsProcessMouseDown(instance, kind);
		}

		static void MouseUp(uint kind)
		{
			auto* instance = gindows::Manager::GetInstance();
			gindowsProcessMouseUp(instance, kind);
		}

		static void MouseWheel(int delta)
		{
			auto* instance = gindows::Manager::GetInstance();
			gindowsProcessMouseWheel(instance, delta);
		}

		static ushort ToGindowsKey(ushort vKey)
		{
			switch (vKey)
			{
			case VK_UP:
				return (ushort)gindows::Key::KEY_UPARROW;

			case VK_DOWN:
				return (ushort)gindows::Key::KEY_DOWNARROW;

			case VK_LEFT:
				return (ushort)gindows::Key::KEY_LEFTARROW;

			case VK_RIGHT:
				return (ushort)gindows::Key::KEY_RIGHTARROW;

			case VK_RETURN:
				return (ushort)gindows::Key::KEY_RETURN;

			case VK_BACK:
				return (ushort)gindows::Key::KEY_BACKSPACE;

			case VK_ESCAPE:
				return (ushort)gindows::Key::KEY_ESCAPE;

			case 0x30:
			case 0x60:
				return (ushort)gindows::Key::KEY_0;
				
			default:
				if (vKey >= 0x41 && vKey <= 0x5A)
				{
					return (vKey - 0x41) + (ushort)gindows::Key::KEY_A;
				}

				if (vKey >= 0x31 && vKey <= 0x39)
				{
					return (vKey - 0x31) + (ushort)gindows::Key::KEY_1;
				}

				if (vKey >= 0x61 && vKey <= 0x69)
				{
					return (vKey - 0x61) + (ushort)gindows::Key::KEY_1;
				}
				
				return vKey;
			}
		}
		
		static void OnKeyDown(ushort vKey)
		{
			gindowsProcessKeyDown(gindows::Manager::GetInstance(), ToGindowsKey(vKey), false, false, false);

			const char map = MapVirtualKeyA(vKey, MAPVK_VK_TO_CHAR);
			if (map)
			{
				gindowsProcessKeyPress(gindows::Manager::GetInstance(), map);
			}
		}

		static void OnKeyUp(ushort vKey)
		{
			gindowsProcessKeyUp(gindows::Manager::GetInstance(), ToGindowsKey(vKey), false, false, false);
		}
	};
}

inline DEFINE_SINGLETONPTR(app::dbg::WindowManager, ASLR(0xFD407C));