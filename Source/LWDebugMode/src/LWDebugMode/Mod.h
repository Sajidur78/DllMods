#pragma once
#include "Configuration.h"

namespace app
{
	class Mod : public csl::fnd::Singleton<Mod>
	{
	protected:
		dev::Configuration m_Config;
		std::string m_Dir;
		
	public:
		Mod();
		Mod(const char* pDir);

		void Initialize(const char* pDir);
		void ApplyPatches() const;
		void OnFrame();
		
		[[nodiscard]] const dev::Configuration& GetConfig() const
		{
			return m_Config;
		}
	};
}
