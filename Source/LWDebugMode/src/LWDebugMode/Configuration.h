#pragma once

namespace app::dev
{
	class Configuration : public fnd::ReferencedObject
	{
	public:
		bool m_EnEditors{};
		
		Configuration() = default;
		void Load(const char* pPath = "Debug.ini");
	};
}
