#include "pch.h"
#include "CameraControllerFreeCam.h"
#include "FreeCameraService.h"

using namespace csl::math;
#undef SendMessage


bool* pRenderHud = reinterpret_cast<bool*>(ASLR(0x00F71B58));
size_t** ppMissionBtn = reinterpret_cast<size_t**>(ASLR(0x00FD9FB0));
FUNCTION_PTR(void, __thiscall, fpMissionListRequestFinish, ASLR(0x00514E30), void* pThis);

namespace app::dev
{
	CameraControllerFreeCam* CameraControllerFreeCam::ms_pInstance{};
	
	CameraControllerFreeCam::CameraControllerFreeCam()
	{
		ms_pInstance = this;
		GetCursorPos(&m_LastTouchPos);
		SetDebugCamera(true);
	}

	CameraControllerFreeCam::~CameraControllerFreeCam()
	{
		ms_pInstance = nullptr;
		*pRenderHud = true;
		ToggleGame(false);
	}

	void CameraControllerFreeCam::ToggleGame(bool pause)
	{
		if (*ppMissionBtn)
		{
			(*(ppMissionBtn))[68] = pause ? 2 : 0;

			void* pMissionList = reinterpret_cast<void*>((*(ppMissionBtn))[71]);
			if (pMissionList)
				fpMissionListRequestFinish(pMissionList);
		}
	}

	void CameraControllerFreeCam::OnEnter(const Camera::EnterEventArgs& rArgs)
	{
		CCameraController::OnEnter(rArgs);
		ToggleGame(true);
		*pRenderHud = false;
		Eigen::Matrix3f lookAt;
		lookAt.col(2) = (m_Position - m_TargetPosition).normalized();
		lookAt.col(0) = m_Up.cross(lookAt.col(2)).normalized();
		lookAt.col(1) = lookAt.col(2).cross(lookAt.col(0));
		
		m_Rotation = Eigen::Quaternionf(lookAt);
		
		m_Up = (m_Rotation * Vector3::UnitY()).normalized();
		m_TargetPosition = Vector3(m_Position + m_Rotation * -Vector3::UnitZ());
	}

	void CameraControllerFreeCam::OnLeave()
	{
		CCameraController::OnLeave();
		ToggleGame(false);
		*pRenderHud = true;
	}

	void CameraControllerFreeCam::Update(const fnd::SUpdateInfo& rInfo)
	{
		PollInputs();
		if (m_ResumeOne)
		{
			m_ResumeOne = false;
			m_Pause = true;
		}
		
		m_Fovy += m_FovAdjust;
		const float yawAdd = m_LookAxis[0];
		const float pitchAdd = m_LookAxis[1];
		const float rollAdd = m_LookAxis[2];

		const Eigen::AngleAxisf x{ MATHF_DEGTORAD(pitchAdd), m_Rotation * Vector3::UnitX() };
		const Eigen::AngleAxisf y{ MATHF_DEGTORAD(-yawAdd), m_Rotation * Vector3::UnitY() };
		const Eigen::AngleAxisf z{ MATHF_DEGTORAD(rollAdd), m_Rotation * Vector3::UnitZ() };

		m_Rotation = (x * y * z * m_Rotation).normalized();
		m_Up = (m_Rotation * Vector3::UnitY()).normalized();
		UpdateCamera(m_MoveAxis);
		
		m_TargetPosition = Vector3(m_Position + m_Rotation * -Vector3::UnitZ());

		if (m_WarpPlayer && FreeCameraService::GetInstance())
		{
			m_WarpPlayer = true;
			const Vector3 forward = (m_TargetPosition - m_Position).normalized();
			FreeCameraService::GetInstance()->SetPlayerPosition(Vector3(m_Position + (forward * 40)));
			ResumeOne();
		}
		
		if (m_Pause)
			ToggleGame(m_Pause);
	}

	void CameraControllerFreeCam::ToggleCamera(GameDocument& rDoc, size_t camera)
	{
		if (!ms_pInstance)
		{
			StartCamera(rDoc, camera);
			return;
		}

		ms_pInstance->DisableCamera();
	}

	void CameraControllerFreeCam::ResumeOne()
	{
		m_ResumeOne = true;
		SetPause(false);
	}

	void CameraControllerFreeCam::SetPause(bool pause)
	{
		m_Pause = pause;
		if (!pause)
			ToggleGame(false);
	}

	void CameraControllerFreeCam::PollInputs()
	{
		float speedFactor{ 1 };
		auto* devMan = csl::fnd::Singleton<hid::DeviceManager>::GetInstance();
		auto* pData = devMan->GetDeviceData(m_Controller, 0);

		if (pData->m_PadData.IsButtonHeld(hid::BUTTON_R2))
			speedFactor = 2;
		else if (pData->m_PadData.IsButtonHeld(hid::BUTTON_L2))
			speedFactor = 0.2f;
		
		m_MoveAxis = GetLStick(pData->m_PadData.ButtonToAxis(hid::BUTTON_R1, hid::BUTTON_L1), m_Speed  * speedFactor);
		m_LookAxis = Vector3(pData->m_PadData.m_RightStick[0], pData->m_PadData.m_RightStick[1], 
			pData->m_PadData.ButtonToAxis(hid::BUTTON_LEFT, hid::BUTTON_RIGHT));
		
		m_FovAdjust = pData->m_PadData.ButtonToAxis(hid::BUTTON_UP, hid::BUTTON_DOWN);
		POINT curCursor{};
		GetCursorPos(&curCursor);
		
		if (pData->m_TouchData.m_HeldStates & hid::TOUCH_PRIMARY)
		{
			const POINT cursorDelta{ curCursor.x - m_LastTouchPos.x, curCursor.y - m_LastTouchPos.y };
			m_LookAxis[0] += static_cast<float>(cursorDelta.x) * 0.08f;
			m_LookAxis[1] -= static_cast<float>(cursorDelta.y) * 0.08f;
		}
		
		if (!pData->m_PadData.IsButtonHeld(hid::BUTTON_R2))
			m_FovAdjust -= ImGui::GetIO().MouseWheel;
		else
			m_LookAxis[2] -= ImGui::GetIO().MouseWheel * m_Speed;
		
		m_WarpPlayer = pData->m_PadData.IsButtonDown(hid::BUTTON_L3);
		if (pData->m_PadData.IsButtonDown(hid::BUTTON_START))
			SetPause(false);
		else if (pData->m_PadData.IsButtonUp(hid::BUTTON_START))
			SetPause(true);

		m_LastTouchPos = curCursor;
	}

	void CameraControllerFreeCam::UpdateCamera(const Vector3& rDir)
	{
		const Vector3 forward = (m_Rotation * -Vector3::UnitZ()).normalized();
		const Vector3 right = (m_Rotation * Vector3::UnitX()).normalized();
		
		const Eigen::Vector3f dir = (forward * rDir[2]) + (m_Up * rDir[1]) + (right * rDir[0]);

		m_Position += dir;
	}

	CameraControllerFreeCam* CameraControllerFreeCam::StartCamera(GameDocument& rDoc, size_t camera)
	{
		if (ms_pInstance)
			return ms_pInstance;

		auto* pCamera = fnd::MessageManager::GetInstance()->GetActor(camera);
		if (!pCamera)
			return nullptr;
		
		auto* pCam = new CameraControllerFreeCam();
		auto msg = xgame::MsgPushCameraController(pCam, 0, false, 9001, 0, false);
		pCamera->SendMessage(msg);

		return pCam;
	}

	Vector3 CameraControllerFreeCam::GetLStick(float height, float speed) const
	{
		auto* devMan = csl::fnd::Singleton<hid::DeviceManager>::GetInstance();
		auto* pData = devMan->GetDeviceData(m_Controller, 0);
		return Vector3(pData->m_PadData.m_LeftStick[0] * speed, height * speed, pData->m_PadData.m_LeftStick[1] * speed);
	}

	void CameraControllerFreeCam::DisableCamera()
	{
		xgame::MsgPopCameraController popMsg(this, 0, false, 0, false);
		m_pOwner->SendMessage(popMsg);
	}
}
