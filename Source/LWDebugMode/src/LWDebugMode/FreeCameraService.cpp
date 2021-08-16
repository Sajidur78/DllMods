#include "pch.h"
#include "FreeCameraService.h"
#include "CameraControllerFreeCam.h"
#undef CreateService
#undef SendMessage

namespace app::dev
{
	const fnd::GameServiceClass FreeCameraService::ms_StaticClass{ "FreeCameraService", CreateService, nullptr };

	fnd::GameService* FreeCameraService::CreateService(csl::fnd::IAllocator* pAlloc)
	{
		return new(pAlloc) FreeCameraService();
	}

	FreeCameraService::FreeCameraService() : GameService(1)
	{
		instance = this;
		SetUpdateFlag(2, true);
	}

	FreeCameraService::~FreeCameraService()
	{
		instance = nullptr;
	}

	void FreeCameraService::OnAddedToGame()
	{
		// Forcefully add our service to the regular group
		auto* pGroup = m_pOwnerDocument->GetGroupActor(13);
		if (pGroup)
			pGroup->AddChild(this);
	}

	bool FreeCameraService::ProcessMessage(fnd::Message& msg)
	{
		if (PreProcessMessage(msg))
			return true;
		
		if (msg.IsOfType<xgame::MsgLevelStarted>())
		{
			LevelStarted();
			return true;
		}
		
		return GameService::ProcessMessage(msg);
	}

	void FreeCameraService::ResolveAccessibleService(GameDocument& document)
	{
		m_pLevelInfo = document.GetService<CLevelInfo>();
	}

	void FreeCameraService::LevelStarted()
	{
		m_IsWorldMap = strcmp(m_pOwnerDocument->GetGameMode()->GetName(), "GameModeWorldAreaMap") == 0;
		m_ActivateButton = m_IsWorldMap ? hid::ButtonStates::BUTTON_L3 : hid::ButtonStates::BUTTON_SELECT;
		xgame::MsgCameraUpdate updateCam(0);
		for (auto& obj : m_pOwnerDocument->GetObjects())
		{
			// Ignore that specific category because of StageTerrainObject
			if (obj->GetObjectCategory() != 13 && obj->SendMessage(updateCam))
			{
				m_CameraActor = obj->GetID();
				break;
			}
		}
	}

	// Update can get hit twice because we exist in 2 groups
	void FreeCameraService::UpdateFinal(fnd::SUpdateInfo& info)
	{
		if (info.frame == m_LastFrame)
			return;

		m_LastFrame = info.frame;
		
		if (CameraControllerFreeCam::GetInstance())
		{
			xgame::MsgCameraUpdate updateCam(info.deltaTime);
			SendMessageImm(m_CameraActor, updateCam);
		}

		auto* devMan = Singleton <hid::DeviceManager>::GetInstance();
		auto* data = devMan->GetDeviceData(0, 0);
		if (data->m_PadData.IsButtonDown(m_ActivateButton))
			CameraControllerFreeCam::ToggleCamera(*m_pOwnerDocument, m_CameraActor);
	}

	void FreeCameraService::SetPlayerPosition(const csl::math::Vector3& rPos) const
	{
		if (!m_pLevelInfo)
			return;

		const auto playerId = m_pLevelInfo->GetPlayerID(0);
		auto* pPlayer = dynamic_cast<Player::CPlayer*>(m_pMessageManager->GetActor(playerId));

		if (!pPlayer)
			return;

		Player::StateUtil::ResetPosition(*pPlayer, rPos);
	}

	void FreeCameraService::SetupService(GameDocument& rDoc)
	{
		if (GetInstance())
			return;

		rDoc.CreateService<FreeCameraService>();
	}
}
