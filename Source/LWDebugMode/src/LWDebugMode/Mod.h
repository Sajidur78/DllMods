#pragma once
#include "Configuration.h"

namespace app
{
	class Mod : public csl::fnd::Singleton<Mod>
	{
	protected:
		struct LevelInfo
		{
			csl::ut::FixedString<64> m_World{};
			csl::ut::FixedString<64> m_LevelTitle{};
			csl::ut::FixedString<16> m_LevelId{};
		};

		static std::vector<LevelInfo> ms_PendingLevels;
		dev::Configuration m_Config;
		std::string m_Dir;
		
	public:
		Mod();
		Mod(const char* pDir);

		void Initialize(const char* pDir);
		void ApplyPatches() const;
		void OnFrame();
		static void AddLevelDebug(const char* pWorldName, const char* pLevelTitle, const char* pLevelId);
		static void SetupPendingLevels();
		
		[[nodiscard]] const dev::Configuration& GetConfig() const
		{
			return m_Config;
		}
	};
}

