#include "pch.h"
#include "CommonFileDialogWin32.h"

namespace app::dev
{
	
#define RETURN_IF_NOT_SUCCESS() if (!SUCCEEDED(hr)) return false

	bool CommonFileDialogWin32::Setup(IFileDialog& rDialog) const
	{	
		rDialog.SetTitle(m_pTitle);
		rDialog.SetDefaultExtension(m_pDefaultExt);
		rDialog.SetFileTypes(m_Filters.size(), m_Filters.begin());
		if (m_FileName)
		{
			const std::string str(m_FileName);
			const std::wstring wStr(str.begin(), str.end());
			rDialog.SetFileName(wStr.c_str());
		}
		
		return true;
	}

	bool CommonFileDialogWin32::PostShow(IFileDialog& rDialog)
	{
		csl::fnd::com_ptr<IShellItem> result;
		HRESULT hr = rDialog.GetResult(reinterpret_cast<IShellItem**>(&result));
		RETURN_IF_NOT_SUCCESS();

		wchar_t* pFileName;
		result->GetDisplayName(SIGDN_FILESYSPATH, &pFileName);
		RETURN_IF_NOT_SUCCESS();

		rDialog.GetFileTypeIndex(&m_SelectedTypeIndex);
		--m_SelectedTypeIndex;

		std::wstring fileName(pFileName);
		const auto aName = std::string(fileName.begin(), fileName.end());

		m_FileName = aName.c_str();
		CoTaskMemFree(pFileName);
		return true;
	}

#undef RETURN_IF_NOT_SUCCESS

	void CommonFileDialogWin32::AddFilter(const wchar_t* pDesc, const wchar_t* pFilter)
	{
		m_Filters.push_back({ pDesc, pFilter });
	}
}
