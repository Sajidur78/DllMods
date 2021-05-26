#pragma once
#include <shtypes.h>
#include "CommonFileDialogWin32.h"

namespace app::dev
{
	class OpenFileDialogWin32 final : public CommonFileDialogWin32
	{
	public:
		bool Show() override;
	};
}
