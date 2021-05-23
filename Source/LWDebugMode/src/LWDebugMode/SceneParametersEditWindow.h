#pragma once
#include "DbgWindowManager.h"

namespace app::xgame
{
	class SceneParametersEditWindow : public dbg::FormObject
	{
	public:
		static const char* ms_pName;
		static dbg::WindowInitNode ms_InitNode;

	protected:
		dev::CReflectionEditControl* m_pEditor{};
		FxSceneData* m_pEditingData{};
		
	public:
		SceneParametersEditWindow();
		static Form* Create();
		static void InstallHooks(bool installInputBlocker = true);
		
	protected:
		void Update(gindows::Object* pSender, gindows::ExecuteEventArgs& args) override;
	};
}
