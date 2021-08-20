#pragma once
#pragma push_macro("CreateService")
#undef CreateService

namespace app::dev
{
	class FreeCameraService : public fnd::GameService, public csl::fnd::Singleton<FreeCameraService>
	{
		static const fnd::GameServiceClass ms_StaticClass;
		static GameService* CreateService(csl::fnd::IAllocator* pAlloc);

	protected:
		CLevelInfo* m_pLevelInfo{};
		size_t m_CameraActor{};
		uint m_LastFrame{};
		bool m_IsWorldMap{};
		hid::ButtonStates m_ActivateButton{};
	
	public:
		size_t m_Controller{};
		FreeCameraService();
		~FreeCameraService() override;
		void OnAddedToGame() override;
		bool ProcessMessage(fnd::Message& msg) override;
		void ResolveAccessibleService(GameDocument& document) override;
		void LevelStarted();
		void UpdateFinal(const fnd::SUpdateInfo& info) override;

		void SetPlayerPosition(const csl::math::Vector3& rPos) const;
		static void SetupService(GameDocument& rDoc);
		static const fnd::GameServiceClass& staticClass() { return ms_StaticClass; }
	};
}

#pragma pop_macro("CreateService")