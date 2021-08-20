#pragma once
#include "CameraControllerFreeCam.h"

namespace app::dev
{
	class Configuration : public fnd::ReferencedObject
	{
	public:
		bool m_EnEditors{};
		bool m_SoftwareCursor{ true };
		struct
		{
			size_t m_Controller{};
			bool m_UseFixedYawAxis{};
			float m_SpeedAdjustRatio{ FREECAM_DEFAULT_SPEED_ADJUST_RATIO };
		} m_FreeCam;
		
		Configuration() = default;
		void Load(const char* pPath = "Debug.ini");
	};
}
