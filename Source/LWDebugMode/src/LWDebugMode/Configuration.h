#pragma once

namespace app::dev
{
	class Configuration : public fnd::ReferencedObject
	{
	public:
		bool m_EnEditors{};
		bool m_SoftwareCursor{ true };
		
		Configuration() = default;
		void Load(const char* pPath = "Debug.ini");
	};
}
