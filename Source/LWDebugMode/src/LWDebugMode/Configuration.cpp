#include "pch.h"
#include "Configuration.h"

namespace app::dev
{
	void Configuration::Load(const char* pPath)
	{
		const INIReader reader{ pPath };
		if (reader.ParseError())
			return;

		m_EnEditors = reader.GetBoolean("Main", "EnEditor", false);
		m_SoftwareCursor = reader.GetBoolean("Main", "SoftwareCursor", true);
		m_FreeCam.m_Controller = reader.GetInteger("FreeCam", "ControllerIndex", 0);
		m_FreeCam.m_UseFixedYawAxis = reader.GetInteger("FreeCam", "UseFixedYawAxis", false);
		m_FreeCam.m_SpeedAdjustRatio = reader.GetFloat("FreeCam", "SpeedAdjustRatio", m_FreeCam.m_SpeedAdjustRatio);
	}
}
