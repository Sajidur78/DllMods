#include "pch.h"
#include "Mod.h"
#include "DbgWindowManager.h"
#include "FreeCameraService.h"

using namespace app;
using namespace app::imgui;
using namespace csl::fnd;

app::CGameSequence::DevData ms_DevData{};

static short WINAPI GetAsyncKeyStateHook(int key)
{
	auto* pInst = gindows::Manager::GetInstance();
	if (pInst && pInst->GetFocusControl() != pInst->GetDesktop())
	{
		// Call the method because, Windows
		GetAsyncKeyState(key);
		return 0;
	}

	return GetAsyncKeyState(key);
}

HOOK(csl::fnd::IAllocator*, __cdecl, GetDebugAllocator, ASLR(0x004438B0))
{
	return app::fnd::GetSingletonAllocator();
}

HOOK(void, __fastcall, WorldMapSetupCamera, ASLR(0x0093EBD0), void* pThis)
{
	originalWorldMapSetupCamera(pThis);

	if (dev::FreeCameraService::GetInstance())
		dev::FreeCameraService::GetInstance()->LevelStarted();
}

HOOK(void, __fastcall, SetupStages, ASLR(0x00913EE0), app::StageInfo::CStageInfo* This)
{
	originalSetupStages(This);
	const auto res = Singleton<fnd::ResourceManager>::GetInstance()->Get<fnd::ResRawData>("actstgdata.lua");

	if (res.IsValid())
	{
		app::game::LuaScript stgData{ nullptr };
		stgData.Load(static_cast<const char*>(res->GetAddress()), res->GetSize());

		This->ReadCategoryDebug("stage_all", stgData, 0);
		This->ReadCategoryDebug("old_stage_all", stgData, 1);
	}

	Mod::SetupPendingLevels();
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

const uint ms_PausedUpdateFlags{ csl::ut::Bitset<uint>{OBJECT_CATEGORY_SERVER, OBJECT_CATEGORY_DEBUG, OBJECT_CATEGORY_CAMERA} };

HOOK(void, __fastcall, StandardGameUpdate, ASLR(0x00926080), void* pThis, void* edx, uint* pInput, const app::fnd::SUpdateInfo& info)
{
	csl::ut::Bitset<int> bits{0, 1, 2};
	
	if (Mod::ms_OverrideUpdateFlow)
	{
		pInput[0] = Mod::ms_UpdateFlags;
		if (Mod::ms_DisablePhysicsFlow)
			pInput[1] = 0x00190100;
		else
			pInput[1] = 0x00190001;
	}

	if (Mod::ms_PauseGame)
	{
		pInput[0] = ms_PausedUpdateFlags;
		pInput[1] = 0x00190100;
	}

	pInput[0] |= Mod::ms_OverlapUpdateFlags;
	originalStandardGameUpdate(pThis, edx, pInput, info);
}

namespace app
{
	std::vector<Mod::LevelInfo> Mod::ms_PendingLevels{};
	bool Mod::ms_PauseGame{};
	bool Mod::ms_OverrideUpdateFlow{};
	bool Mod::ms_DisablePhysicsFlow{};
	csl::ut::Bitset<uint> Mod::ms_OverlapUpdateFlags{};
	csl::ut::Bitset<uint> Mod::ms_UpdateFlags
	{
		1 << OBJECT_CATEGORY_SERVER |
		1 << OBJECT_CATEGORY_NONSTOP |
		1 << OBJECT_CATEGORY_CAMERA |
		1 << OBJECT_CATEGORY_HUD |
		1 << OBJECT_CATEGORY_HUD_NONSTOP |
		1 << OBJECT_CATEGORY_DEBUG
	};
	
	Mod::Mod()
	{
		ReplaceInstance(this);
	}

	Mod::Mod(const char* pDir)
	{
		ReplaceInstance(this);
		Initialize(pDir);
	}

	void Mod::Initialize(const char* pDir)
	{
		m_Config.Load();
		m_Dir = pDir;
	}

	void Mod::ApplyPatches() const
	{
		WindowManager::SetCurrentDir(m_Dir.c_str());
		if (m_Config.m_EnEditors)
		{
			WRITE_MEMORY(ASLR(0x00458351), 0x90, 0x90, 0x90, 0x90, 0x90)
			WRITE_MEMORY(ASLR(0x0045835A), 0x90, 0x90, 0x90, 0x90, 0x90)
			WRITE_MEMORY(ASLR(0x0045835A), 0x31, 0xC0)
		}

		// Fix load test crashes
		WRITE_MEMORY(ASLR(0x0092AAA6), 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90)
		WRITE_MEMORY(ASLR(0x0092AAF6), 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90)
		// WRITE_MEMORY(ASLR(0x00418B2C), 12)
		
		WRITE_FUNCTION(ASLR(0x00D52234), &GetAsyncKeyStateHook)
		INSTALL_HOOK(GetDebugAllocator)
		INSTALL_HOOK(SetupStages)
		INSTALL_HOOK(StateDevMenu)
		INSTALL_HOOK(GameModeDevMenuCtor)
		INSTALL_HOOK(WorldMapSetupCamera)
		INSTALL_HOOK(StandardGameUpdate)
	}

	void Mod::OnFrame()
	{
		if (GameDocument::GetSingleton())
			dev::FreeCameraService::SetupService(*GameDocument::GetSingleton());
		
		WindowManager::GetInstance()->Render();
		auto* devMan = Singleton <hid::DeviceManager>::GetInstance();

		if (!devMan)
			return;

		auto* data = devMan->GetDeviceData(0, 0);
		const hid::ButtonStates buttons = data->m_PadData.m_HeldButtons;

		if (buttons & hid::ButtonStates::BUTTON_L1
			&& buttons & hid::ButtonStates::BUTTON_L3
			&& buttons & hid::ButtonStates::BUTTON_R1)
		{
			Singleton<WindowManager>::GetInstance()->SeqGoToDevMenu();
		}
	}

	void Mod::AddLevelDebug(const char* pWorldName, const char* pLevelTitle, const char* pLevelId)
	{
		if (!pWorldName || !pLevelTitle || !pLevelId)
			return;

		auto* pInst = app::StageInfo::CStageInfo::GetInstance();
		if (!pInst)
		{
			LevelInfo info{};
			info.m_World = pWorldName;
			info.m_LevelTitle = pLevelTitle;
			info.m_LevelId = pLevelId;
			ms_PendingLevels.push_back(info);
			return;
		}

		auto& worlds = pInst->m_Worlds[0];
		app::StageInfo::WorldNode* pWorld{};
		for (auto& zone : worlds)
		{
			if (!strncmp(zone->GetTitle(), pWorldName, strlen(pWorldName)))
			{
				pWorld = zone;
				break;
			}
		}

		if (!pWorld)
		{
			pWorld = new(worlds.get_allocator()) app::StageInfo::WorldNode();
			pWorld->SetTitle(pWorldName);
			worlds.push_back(pWorld);
		}

		app::StageInfo::StageNode* pStageData{};
		for (auto& stage : pWorld->GetStages())
		{
			if (!strncmp(stage.m_Name, pLevelId, 16))
			{
				pStageData = &stage;
				pStageData->m_Name = pLevelId;
				pStageData->m_Title = pLevelTitle;
				break;
			}
		}

		if (!pStageData)
		{
			StageInfo::StageNode stageData{};
			stageData.m_Name = pLevelId;
			stageData.m_Title = pLevelTitle;
			pWorld->AddNode(stageData);
		}
	}

	void Mod::SetupPendingLevels()
	{
		if (!StageInfo::CStageInfo::GetInstance())
			return;
		
		for (auto& level : ms_PendingLevels)
		{
			AddLevelDebug(level.m_World.c_str(), level.m_LevelTitle.c_str(), level.m_LevelId.c_str());
		}
		
		ms_PendingLevels.clear();
	}
}
