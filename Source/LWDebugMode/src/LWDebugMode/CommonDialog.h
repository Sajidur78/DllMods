#pragma once

namespace app::dev
{
	class CommonDialog : public fnd::ReferencedObject
	{
	public:
		virtual bool Show() = 0;
		
		CommonDialog()
		{
			
		}
	};
}