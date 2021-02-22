#include "pch.h"
#include "DummyDevice.h"

#include "WindowManager.h"

using namespace csl::fnd;
using namespace app::hid;
using namespace app::dbg;

uint app::dbg::hid::DummyDevice::GetPortMax() const
{
	//Singleton<DeviceWin>::GetInstance()->RemoveDevice(this);
	
	Singleton<imgui::WindowManager>::GetInstance()->Initialize();
	
	return 1;
}
