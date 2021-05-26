#include "pch.h"
#include "OpenFileDialogWin32.h"
#include <shobjidl.h>
#include "commdlg.h"

namespace app::dev
{
	bool OpenFileDialogWin32::Show()
	{
#define RETURN_IF_NOT_SUCCESS() if (!SUCCEEDED(hr)) return false
		
		csl::fnd::com_ptr<IFileDialog> pFileDialog;
		auto hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER,
		                           IID_PPV_ARGS(((IFileDialog**)&pFileDialog)));
		
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
