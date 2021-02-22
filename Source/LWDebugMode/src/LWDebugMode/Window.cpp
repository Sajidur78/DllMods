#include "pch.h"
#include "Window.h"

void* app::imgui::Window::operator new(size_t size)
{
	return ReferencedObject::operator new(size, WindowManager::GetMemoryAllocator());
}
