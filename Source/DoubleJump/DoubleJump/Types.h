#pragma once
#include <cstdint>
#include "Functions.h"

#define STATIC_ASSERT_SIZEOF(type, size) \
	static_assert(sizeof(type) == (size), "sizeof("#type") != "#size"");

struct GensString
{
	char* value;
	uint32_t length;

	GensString() : value(nullptr), length(0)
	{
	}

	GensString(const char* value) : GensString()
	{
		constructGensString(this, value);
	}
};

STATIC_ASSERT_SIZEOF(GensString, 8);