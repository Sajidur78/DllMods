#pragma once
#include <shtypes.h>
#include <shobjidl.h>
#include "CommonDialog.h"

namespace app::dev
{
	class CommonFileDialogWin32 : CommonDialog
	{
	public:
		const wchar_t* m_pTitle{};
		const wchar_t* m_pDefaultExt{};
		csl::ut::VariableString m_FileName{ fnd::GetTempAllocator() };

	protected:
		csl::ut::InplaceMoveArray<COMDLG_FILTERSPEC, 1> m_Filters{ fnd::GetTempAllocator() };
		size_t m_SelectedTypeIndex{ static_cast<size_t>(-1) };
		bool Setup(IFileDialog& rDialog) const;
		bool PostShow(IFileDialog& rDialog);
		
	public:

		HH_FORCE_INLINE void SetFileName(const char* pName)
		{
			m_FileName = pName;
		}
		
		HH_FORCE_INLINE void SetTitle(const wchar_t* pTitle)
		{
			m_pTitle = pTitle;
		}

		HH_FORCE_INLINE void SetDefaultExtension(const wchar_t* pExt)
		{
			m_pDefaultExt = pExt;
		}

		HH_FORCE_INLINE size_t GetFileTypeIndex() const
		{
			return m_SelectedTypeIndex;
		}

		void AddFilter(const wchar_t* pDesc, const wchar_t* pFilter);
	};
}
