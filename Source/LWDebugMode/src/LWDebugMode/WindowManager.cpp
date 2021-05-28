#include "pch.h"
#include "WindowManager.h"
#include "DbgWindowManager.h"
#include "HioServer.h"
#include <string>
#include <windowsx.h>

#pragma once
#pragma push_macro("SendMessage")
#undef SendMessage

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace app::xgame;
using namespace app::game;
using namespace app::hid;
using namespace csl::fnd;

namespace app::imgui
{
	static bool* ms_pRenderHud = reinterpret_cast<bool*>(0x00F71B58);
	static FUNCTION_PTR(void, __thiscall, SetEnableViewerCamera, ASLR(0x00475BF0), void* viewerManager, bool enable);

	void* WindowManager::WindowManager_init()
	{
		auto* allocator = fnd::GetSingletonAllocator();

		gindows::SetMemoryAllocator(fnd::GetSingletonAllocator());
		gindows::Manager::Initialize();

		GlobalAllocator::SetAllocator(3, fnd::GetSingletonAllocator());

		new(GetMemoryAllocator()) dbg::HioServer();

		auto* mgr = new(fnd::GetSingletonAllocator()) WindowManager();
		mgr->AddRef();

		return mgr;
	}

	void WindowManager::WindowManager_destroy(void* instance)
	{
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		delete static_cast<BaseObject*>(instance);
	}

	LRESULT CALLBACK WindowManagerProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);
		
		switch (Msg)
		{
		case WM_KEYDOWN:
		{
			if (wParam == VK_F5)
			{
				Singleton<WindowManager>::GetInstance()->SeqGoToDevMenu();
			}
			else if (wParam == VK_F3)
			{
				auto* pWindowMan = Singleton<dbg::WindowManager>::GetInstance();
				if (pWindowMan)
					pWindowMan->ToggleWindowByName("FxParamEdit");
			}
				
			dbg::WindowManager::OnKeyDown(static_cast<ushort>(wParam));
			break;
		}

		case WM_KEYUP:
		{
			dbg::WindowManager::OnKeyUp(static_cast<ushort>(wParam));
			break;
		}

		case WM_LBUTTONDOWN:
		{
			dbg::WindowManager::MouseDown(1);
			break;
		}

		case WM_LBUTTONUP:
		{
			dbg::WindowManager::MouseUp(1);
			break;
		}

		case WM_RBUTTONDOWN:
		{
			dbg::WindowManager::MouseDown(2);
			break;
		}

		case WM_RBUTTONUP:
		{
			dbg::WindowManager::MouseUp(2);
			break;
		}

		case WM_MOUSEWHEEL:
		{
			dbg::WindowManager::MouseWheel(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
			break;
		}

		case WM_SETCURSOR:
		{
			if (LOWORD(lParam) == HTCLIENT && ::GetActiveWindow() == hWnd)
			{
				SetCursor(nullptr);
				return true;
			}
			break;
		}

		case WM_MOUSEMOVE:
		{
			const csl::ut::Point2<int> point{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			dbg::WindowManager::MouseMove(point);
			if (gindows::Manager::GetInstance() && gindows::Manager::GetInstance()->GetMouseOverControl() != gindows::Manager::GetInstance()->GetDesktop())
				return true;
				
			break;
		}

		default:
			break;
		}

		return CallWindowProc((WNDPROC)WindowManager::WndProcOriginal, hWnd, Msg, wParam, lParam);
	}

	WindowManager::WindowManager()
	{
		Singleton<font::FontManager>::GetInstance()->SetDbgFont(&m_MainFont);
		Singleton<fnd::MessageManager>::GetInstance()->Add(this);

		new(GetAllocator()) dbg::WindowManager();

		MsgNotifyObjectEvent msg(WM_EVENT_INIT);
		SendMessage(m_ActorID, msg);
	}

	void WindowManager::SeqGoToDevMenu()
	{
		auto* pSeq = ApplicationWin::GetInstance()->GetGame()->GetSequence();
		if (pSeq)
		{
			pSeq->m_pNextSequence = reinterpret_cast<void*>(ASLR(0x0090FEF0)); // StateDevMenu
			pSeq->m_Unk2 = nullptr;
			pSeq->m_Unk3 = nullptr;
		}
	}

	void WindowManager::Initialize()
	{
		if (m_initialized)
			return;

		auto* app = ApplicationWin::GetInstance();

		WndProcOriginal = SetWindowLongPtr((HWND)app->GetWindowHandle(), GWLP_WNDPROC, (LONG_PTR)WindowManagerProc);

		ImGui::CreateContext();
		ImGui_ImplDX9_Init(app->GetDirect3DDevice());
		ImGui_ImplWin32_Init(reinterpret_cast<void*>(app->GetWindowHandle()));

		ImGuiIO& io = ImGui::GetIO();
		//io.MouseDrawCursor = true;

		const std::filesystem::path path = ms_CurDir / "Fonts" / "DroidSans.ttf";
		const std::filesystem::path pathJap = ms_CurDir / "Fonts" / "DroidSansJapanese.ttf";

		m_MainFont.SetFont(io.Fonts->AddFontFromFileTTF(path.u8string().c_str(), 20, nullptr, io.Fonts->GetGlyphRangesDefault()));

		ImFontConfig fontConfig;
		fontConfig.MergeMode = true;
		
		m_MainFont.SetFont(io.Fonts->AddFontFromFileTTF(pathJap.u8string().c_str(), 20, &fontConfig, io.Fonts->GetGlyphRangesJapanese()));

		m_initialized = true;
	}

	void WindowManager::Render()
	{
		if (!m_initialized)
			return;

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		dbg::WindowManager::Render();
		
		for (auto& window : m_windows)
		{
			if (window->m_isVisible)
			{
				ImGui::SetNextWindowPos(window->GetPosition(), ImGuiCond_Appearing);
				if (ImGui::Begin(window->GetTitle().c_str(), &window->m_isVisible, window->GetWindowFlags()))
					window->Render();

				ImGui::End();
			}
		}

		Singleton<font::FontManager>::GetInstance()->DbgDraw();
		
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	bool WindowManager::ProcessMessage(fnd::Message& msg)
	{
		if (PreProcessMessage(msg))
			return true;

		if (msg.IsOfType<MsgNotifyObjectEvent>())
		{
			switch (static_cast<MsgNotifyObjectEvent&>(msg).GetEventType())
			{
			case WM_EVENT_INIT:
				Initialize();
				break;

			default:
				return false;
			}
		}

		return false;
	}

	DEFINE_SINGLETON(WindowManager);
}

#pragma pop_macro("SendMessage")