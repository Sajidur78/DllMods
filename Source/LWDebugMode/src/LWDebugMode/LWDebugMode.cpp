#include "pch.h"
#include "LWDebugMode.h"
#include "DbgWindowManager.h"
#include "Configuration.h"

#pragma comment(lib, "lib/detours.lib")
#pragma comment(lib, "lib/syelog.lib")

using namespace app::imgui;
using namespace csl::fnd;

app::CGameSequence::DevData ms_DevData{};
app::dev::Configuration ms_Config;

static short WINAPI GetAsyncKeyStateHook(int key)
{
	if (gindows::Manager::GetInstance() && gindows::Manager::GetInstance()->GetFocusControl() != gindows::Manager::GetInstance()->GetDesktop())
		return 0;

	return GetAsyncKeyState(key);
}

HOOK(csl::fnd::IAllocator*, __cdecl, GetDebugAllocator, ASLR(0x004438B0))
{
	return app::fnd::GetSingletonAllocator();
}

HOOK(void, __fastcall, SetupStages, ASLR(0x00913EE0), app::StageInfo::CStageInfo* This)
{
	originalSetupStages(This);
	const auto res = Singleton<app::fnd::ResourceManager>::GetInstance()->Get<app::fnd::ResRawData>("actstgdata.lua");

	if (res.IsValid())
	{
		app::game::LuaScript stgData{ nullptr };
		stgData.Load(static_cast<const char*>(res->GetAddress()), res->GetSize());

		This->ReadCategoryDebug("stage_all", stgData, 0);
		This->ReadCategoryDebug("old_stage_all", stgData, 1);
	}
}

HOOK(bool, __fastcall, StateDevMenu, ASLR(0x0090FEF0), app::CGameSequence* This, void* edx, void* a2, void* a3)
{
	if (!This->GetDevData())
		This->SetDevData(&ms_DevData);

	return originalStateDevMenu(This, edx, a2, a3);
}

HOOK(void*, __fastcall, GameModeDevMenuCtor, ASLR(0x00914890), void* This)
{
	auto* pWindowMan = Singleton<app::dbg::WindowManager>::GetInstance();

	if (pWindowMan)
		pWindowMan->CreateWindowByName("DevConfig");
	
	return originalGameModeDevMenuCtor(This);
}

extern "C" __declspec(dllexport) void OnFrame()
{
	Singleton<WindowManager>::GetInstance()->Render();
	auto* devMan = Singleton <app::hid::DeviceManager>::GetInstance();

	if (!devMan)
		return;

	auto* data = devMan->GetDeviceData(0, 0);
	const app::hid::ButtonStates buttons = data->m_PadData.m_HeldButtons;
	
	if (buttons & app::hid::ButtonStates::BUTTON_L1 
		&& buttons & app::hid::ButtonStates::BUTTON_L3
		&& buttons & app::hid::ButtonStates::BUTTON_R1)
	{
		Singleton<WindowManager>::GetInstance()->SeqGoToDevMenu();
	}
}

extern "C" __declspec(dllexport) void Init()
{
	char pathBuf[MAX_PATH];
	GetCurrentDirectoryA(sizeof(pathBuf), pathBuf);
	ms_Config.Load();
	
	if (ms_Config.m_EnEditors)
	{
		// WriteNop(ASLR(0x00458351), 5);
		// WriteNop(ASLR(0x0045835A), 5);
		// WriteProtected<ushort>(ASLR(0x0045835A), 0xC031); // xor eax, eax
		
		WRITE_MEMORY(ASLR(0x00458351), 0x90, 0x90, 0x90, 0x90, 0x90);
		WRITE_MEMORY(ASLR(0x0045835A), 0x90, 0x90, 0x90, 0x90, 0x90);
		WRITE_MEMORY(ASLR(0x0045835A), 0x31, 0xC0);
	}
	
	// Fix load test crashes
	WRITE_MEMORY(ASLR(0x0092AAA6), 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90)
	WRITE_MEMORY(ASLR(0x0092AAF6), 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90)
	
	WindowManager::SetCurrentDir(pathBuf);

	WRITE_FUNCTION(ASLR(0x00D52234), &GetAsyncKeyStateHook);
	INSTALL_HOOK(GetDebugAllocator)
	INSTALL_HOOK(SetupStages)
	INSTALL_HOOK(StateDevMenu)
	INSTALL_HOOK(GameModeDevMenuCtor)
}