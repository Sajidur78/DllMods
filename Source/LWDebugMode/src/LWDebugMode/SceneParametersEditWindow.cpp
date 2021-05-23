#include "pch.h"
#include "SceneParametersEditWindow.h"

using namespace csl::ut;
using namespace app::dev;
using namespace app::dbg;

HOOK(void, __fastcall, FxColManagerUpdate, ASLR(0x00904E00), app::fnd::CActor* pThis, void* edx, app::fnd::SUpdateInfo& rUpdate)
{
	auto* pMan = static_cast<app::FxColManager*>(pThis);
	if (!pMan->IsEditingFxParam())
		originalFxColManagerUpdate(pThis, edx, rUpdate);
}

static short WINAPI GetAsyncKeyStateHook(int key)
{
	if (gindows::Manager::GetInstance() && gindows::Manager::GetInstance()->GetFocusControl() != gindows::Manager::GetInstance()->GetDesktop())
		return 0;
	
	return GetAsyncKeyState(key);
}

namespace app::xgame
{
	SceneParametersEditWindow::SceneParametersEditWindow() : FormObject(ms_pName)
	{
		m_pEditor = new CReflectionEditControl(nullptr);
		Form::Add(m_pEditor);
		m_pEditor->SetFocus();
		
		const Point2<int> location{ 300, 100 };
		const Size2<int> size{ 480, 320 };
		SetSize(size);
		SetLocation(location);
		SceneParameters::GetInstance();
	}

	gindows::Form* SceneParametersEditWindow::Create()
	{
		return new SceneParametersEditWindow();
	}

	void SceneParametersEditWindow::InstallHooks(bool installInputBlocker)
	{
		WRITE_FUNCTION(ASLR(0x00D52234), &GetAsyncKeyStateHook);
		INSTALL_HOOK(FxColManagerUpdate)
	}

	void SceneParametersEditWindow::Update(Object* pSender, gindows::ExecuteEventArgs& args)
	{
		if (!SceneParameters::GetInstance())
		{
			if (m_pEditingData)
			{
				m_pEditor->Reset();
				m_pEditingData = nullptr;
			}
			return;
		}

		if (SceneParameters::GetInstance() && FxColManager::GetInstance() && FxColManager::GetInstance()->IsEditingFxParam())
			SceneParameters::GetInstance()->ApplyParameter();
		
		auto* pInst = SceneParameters::GetInstance();
		auto* pData = pInst->GetEditingData();
		if (!pData || pData == m_pEditingData)
			return;

		m_pEditingData = pData;

		pInst->SetData(pData);
		const fnd::Variant data{ pData, &FxSceneData::staticClass() };
		m_pEditor->SetData(data);

		if (FxColManager::GetInstance())
			FxColManager::GetInstance()->ToggleEditingFxParam(true);
	}
}

const char* app::xgame::SceneParametersEditWindow::ms_pName = "FxParamEdit";
WindowInitNode app::xgame::SceneParametersEditWindow::ms_InitNode{ ms_pName, Create };