#pragma once

namespace app::dev
{
	class CameraControllerFreeCam : public Camera::CCameraController
	{
		static CameraControllerFreeCam* ms_pInstance;
		csl::math::Quaternion m_Rotation{};
		csl::math::Vector3 m_MoveAxis{};
		csl::math::Vector3 m_LookAxis{};
		float m_FovAdjust{};
		bool m_WarpPlayer{};
		POINT m_LastTouchPos{};
	
	public:
		size_t m_Controller{};
		float m_Speed{ 2.5f };
		CameraControllerFreeCam();
		~CameraControllerFreeCam() override;
		
		static void ToggleGame(bool pause);
		void OnEnter(const Camera::EnterEventArgs& rArgs) override;
		void OnLeave() override;
		void Update(const app::fnd::SUpdateInfo& rInfo) override;
		static void ToggleCamera(GameDocument& rDoc, size_t camera);
		static CameraControllerFreeCam* StartCamera(GameDocument& rDoc, size_t camera);
		static CameraControllerFreeCam* GetInstance()
		{
			return ms_pInstance;
		}
	
	protected:
		bool m_Pause{ true };
		bool m_ResumeOne{ false };
		void ResumeOne();
		void SetPause(bool pause);
		void PollInputs();
		void UpdateCamera(const csl::math::Vector3& rDir);
		[[nodiscard]] csl::math::Vector3 GetLStick(float height = 0, float speed = 1) const;
		void DisableCamera();
	};
}
