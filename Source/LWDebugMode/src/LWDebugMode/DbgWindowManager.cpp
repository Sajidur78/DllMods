#include "pch.h"
#include "DbgWindowManager.h"

void app::dbg::WindowManager::FormDestroyEventHandler(gindows::Object* pControl, gindows::EventArgs& args)
{
	RemoveWindow(reinterpret_cast<gindows::Control*>(pControl));
}

gindows::Form* app::dbg::WindowManager::CreateWindowByName(const char* pName)
{
	gindows::Form* pWindow = FindWindowByName(pName);

	if (pWindow)
		return pWindow;
	
	WindowInitNode* pNode = WindowInitNode::FindByName(pName);
	if (!pNode)
		return nullptr;

	pWindow = pNode->m_pConstructor();

	if (!pWindow)
		return nullptr;

	pWindow->m_OnDestroy += csl::ut::MakePair(this, &WindowManager::FormDestroyEventHandler);
	
	AddWindow(pWindow);
	return pWindow;
}
