#pragma once

namespace app::dbg
{
	class HioServer : public fnd::ReferencedObject, csl::fnd::SingletonPointer<HioServer>
	{
		inline static FUNCTION_PTR(void, __thiscall, ms_fpHioServerCtor, ASLR(0x0096D570), void* This, csl::fnd::IAllocator* allocator);
		char m_Server[8192]{};
		
	public:
		HioServer()
		{
			ms_fpHioServerCtor(m_Server, GetAllocator());
			ReplaceInstance(this);
		}
	};
}

inline DEFINE_SINGLETONPTR(app::dbg::HioServer, ASLR(0xFD409C));