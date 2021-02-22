#include "pch.h"
#include "DbgWindowManager.h"
#include "DbgWindows.h"

void app::dbg::WindowManager::FormDestroyEventHandler(gindows::Object* pControl, gindows::EventArgs& args)
{
	if (GetInstance())
	{
		GetInstance()->RemoveWindow(reinterpret_cast<gindows::Control*>(pControl));
	}
}

gindows::Control* app::dbg::WindowManager::CreateWindowByName(const char* pName)
{
	gindows::Control* pWindow = FindWindowByName(pName);

	if (pWindow)
		return pWindow;
	
	WindowInitNode* pNode = WindowInitNode::FindByName(pName);
	if (!pNode)
		return nullptr;

	pWindow = pNode->m_pConstructor();

	if (!pWindow)
		return nullptr;

	pWindow->m_OnDestroy += FormDestroyEventHandler;
	
	AddWindow(pWindow);
	return pWindow;
}
