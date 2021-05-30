#pragma once
#pragma push_macro("SendMessage")
#undef SendMessage

#include <filesystem>

namespace app::imgui
{
	using namespace xgame;
	using namespace std;
	
	class WindowManager : public fnd::ReferencedObject, fnd::CLeafActor, public csl::fnd::Singleton<WindowManager>
	{
		static constexpr uint WM_EVENT_INIT = 1;

		inline static std::filesystem::path ms_CurDir{};
		inline static ImGuiAllocator ms_ImGuiAllocator{};

		static void* WindowManager_init();
		static void WindowManager_destroy(void* instance);

		DECLARE_SINGLETON

		bool m_initialized{ false };
		list<Window*> m_windows{};
		font::ImGuiFont m_MainFont{};

	public:
		inline static LONG_PTR WndProcOriginal;

		WindowManager();

		~WindowManager() override
		{
			m_pMessageManager->Remove(this);

			for (auto& window : m_windows)
			{
				window->Release();
			}
		}

		void SeqGoToDevMenu();
		void Initialize();
		void Render();
		bool ProcessMessage(fnd::Message& msg) override;

		void AddWindow(Window* window)
		{
			if (!window)
				return;

			m_windows.push_back(window);
			window->AddRef();
		}

		void RemoveWindow(Window* window)
		{
			m_windows.remove(window);
			window->Release();
		}

		static void SetCurrentDir(const char* path)
		{
			ms_CurDir = path;
		}

		static csl::fnd::IAllocator* GetMemoryAllocator()
		{
			return &ms_ImGuiAllocator;
		}
	};
}

#pragma pop_macro("SendMessage")