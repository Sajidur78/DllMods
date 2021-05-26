#pragma once
#include "CommonFileDialogWin32.h"

namespace app::dev
{
	class SaveFileDialogWin32 final : public CommonFileDialogWin32
	{
	public:
		bool Show() override;
	};
}
