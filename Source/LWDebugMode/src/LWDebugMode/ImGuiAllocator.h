#pragma once

namespace app::imgui
{
	class ImGuiAllocator : public csl::fnd::IAllocator
	{
		void* Alloc(size_t size, int alignment) override
		{
			return _aligned_malloc(size, alignment);
		}

		void Free(void* loc) override
		{
			_aligned_free(loc);
		}
	};
}
