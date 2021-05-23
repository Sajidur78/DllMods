#pragma once
#include "DbgWindowManager.h"

namespace app::xgame
{
	enum LauguageType
	{
		JAPANESE= 0,
		ENGLISH = 1,
		GERMAN = 2,
		FRENCH = 3,
		SPANISH = 4,
		ITALIAN = 5,
		DUTCH = 6,
	};
	
	struct DevConfigParameters
	{
		LauguageType language;
		RcType rcType;
		short rcDesign;
		short rcLevel;
		bool battleMode;
		short ringRaceSetNo;
		bool minigameMode;
	};
	
	class DevConfigEditWindow : public dbg::FormObject
	{
	public:
		inline static const char* ms_pName = "DevConfig";
		
	protected:
		dev::CReflectionEditControl* m_pEditor{};
		DevConfigParameters* m_pParameters{};
		
	public:
		DevConfigEditWindow();

		void OnKeyDown(gindows::KeyEventArgs& args) override
		{
			if (!args.m_Handled)
			{
				// ???, it just does that
				args.m_Unk2++;
				m_pEditor->OnKeyDown(args);
				args.m_Unk2--;
			}

			FormObject::OnKeyDown(args);
		}

		static Form* Create();
		inline static dbg::WindowInitNode ms_FormInitNode{ ms_pName, Create };
	};
}