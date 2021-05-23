#include "pch.h"
#include "DevConfigEditWindow.h"

using namespace app::dbg;
using namespace app::dev;

namespace app::xgame
{
	DevConfigEditWindow::DevConfigEditWindow() : FormObject(ms_pName)
	{
		m_pEditor = new CReflectionEditControl(nullptr);
		if (m_pEditor)
			Control::Add(m_pEditor);

		const csl::ut::Size2<int> size{ 480, 240 };
		const csl::ut::Point2<int> point{ 700, 100 };

		SetSize(size);
		SetLocation(point);
	}

	gindows::Form* DevConfigEditWindow::Create()
	{
		return new DevConfigEditWindow();
	}
}
