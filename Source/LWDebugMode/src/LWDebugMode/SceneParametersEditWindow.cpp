#include "pch.h"
#include "SceneParametersEditWindow.h"
#include "OpenFileDialogWin32.h"
#include "SaveFileDialogWin32.h"

using namespace csl::ut;
using namespace app::dev;
using namespace app::dbg;
using namespace app::fnd;
using namespace csl::fnd;

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
		m_OnDestroy += MakePair(this, &SceneParametersEditWindow::OnDestroy);
		m_pEditor = new CReflectionEditControl(nullptr);
		Form::Add(m_pEditor);
		m_pEditor->SetFocus();
		m_pEditor->m_OnKeyDown += MakePair(this, &SceneParametersEditWindow::edit_KeyDown);

		auto* pDesktop = gindows::Manager::GetInstance()->GetDesktop();
		const auto deskSize = pDesktop->GetSize();
		const Size2<int> size{ (static_cast<float>(deskSize.width) / 2.5f) - 25, deskSize.height - 50 };
		const Point2<int> location{ (deskSize.width - size.width) - 50, 25 };
		
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
		if (installInputBlocker)
		{
			WRITE_FUNCTION(ASLR(0x00D52234), &GetAsyncKeyStateHook);
		}

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

	void SceneParametersEditWindow::OnDestroy(Object*, gindows::EventArgs&)
	{
		if (FxColManager::GetInstance())
			FxColManager::GetInstance()->ToggleEditingFxParam(false);
	}

	void SceneParametersEditWindow::edit_KeyDown(Object* pSender, gindows::KeyEventArgs& args)
	{
		auto* pParameters = SceneParameters::GetInstance();
		if (!pParameters)
			return;

		switch (static_cast<gindows::Key>(args.m_KeyCode))
		{
		case gindows::Key::KEY_O:
		{
			args.m_Handled = true;
			OpenData();
			break;
		}

		case gindows::Key::KEY_S:
			args.m_Handled = true;
			SaveData();
			break;

		case gindows::Key::KEY_B:
			pParameters->Backup();
			args.m_Handled = true;
			break;

		case gindows::Key::KEY_R:
			pParameters->Restore();
			m_pEditor->ReadValue();
			args.m_Handled = true;
			break;

		default:
			break;
		}
	}

	void SceneParametersEditWindow::OpenData()
	{
		auto* pSeq = ApplicationWin::GetInstance()->GetGame()->GetSequence();
		OpenFileDialogWin32 ofd{};

		ofd.SetTitle(L"Open Scene Data");
		ofd.SetFileName(pSeq->m_StgId);
		ofd.AddFilter(L"Hedgehog Scene Data", L"*.hhd");
		ofd.AddFilter(L"Hedgehog Scene Config", L"*.scn-cfg.xml");

		if (!ofd.Show())
			return;

		const ut::RefPtr<IOStream> inputStream{ Singleton<HostFileSystem>::GetInstance()->OpenFile(ofd.m_FileName) };
		if (!inputStream)
			return;

		switch (ofd.GetFileTypeIndex())
		{
		case 0:
		{
			FxSceneData dataBuf{};
			const size_t bytesRead = inputStream->Read(&dataBuf, sizeof(dataBuf));
			if (bytesRead != sizeof(dataBuf))
				break;

			*m_pEditingData = dataBuf;
			break;
		}

		case 1:
		{
			const ut::RefPtr<DataResource> resource = RflSerializeUtil::Load(inputStream, GetTempAllocator());
			if (!resource)
				break;

			*m_pEditingData = *resource->GetContents<FxSceneData>();
		}
		default:
			break;
		}

		m_pEditor->ReadValue();
	}

	void SceneParametersEditWindow::SaveData()
	{
		SaveFileDialogWin32 sfd{};

		auto* pSeq = ApplicationWin::GetInstance()->GetGame()->GetSequence();
		
		sfd.SetTitle(L"Save Scene Data");
		sfd.SetFileName(pSeq->m_StgId);
		sfd.SetDefaultExtension(L"hhd");
		sfd.AddFilter(L"Hedgehog Scene Data", L"*.hhd");
		sfd.AddFilter(L"Hedgehog Scene Config", L"*.scn-cfg.xml");

		if (!sfd.Show())
			return;

		const ut::RefPtr<IOStream> outStream{ Singleton<HostFileSystem>::GetInstance()->CreateFile(sfd.m_FileName) };
		if (!outStream)
			return;

		switch (sfd.GetFileTypeIndex())
		{
		case 0:
			outStream->Write(m_pEditingData, sizeof(FxSceneData));
			break;

		case 1:
			RflSerializeUtil::Save(Variant{ m_pEditingData, &FxSceneData::staticClass() }, outStream);
			break;

		default:
			break;
		}
	}
}

const char* app::xgame::SceneParametersEditWindow::ms_pName = "FxParamEdit";
WindowInitNode app::xgame::SceneParametersEditWindow::ms_InitNode{ ms_pName, Create };