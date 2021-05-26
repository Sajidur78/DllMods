#include "pch.h"
#include "SaveFileDialogWin32.h"
#include <shobjidl.h>

namespace app::dev
{
	bool SaveFileDialogWin32::Show()
	{
#define RETURN_IF_NOT_SUCCESS() if (!SUCCEEDED(hr)) return false

		csl::fnd::com_ptr<IFileSaveDialog> pFileDialog;
		auto hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(((IFileSaveDialog**)&pFileDialog)));

		RETURN_IF_NOT_SUCCESS();

		if (!Setup(*pFileDialog))
			return false;
		
		hr = pFileDialog->Show(nullptr);
		RETURN_IF_NOT_SUCCESS();
		
		if (!PostShow(*pFileDialog))
			return false;
		
		return true;

#undef RETURN_IF_NOT_SUCCESS
	}
}
