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
	}
}
