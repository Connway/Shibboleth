/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

#pragma once

#include <Gaff_Platform.h>
#include <cstdint> // For (u)int*_t and size_t

#define NS_GLEAM namespace Gleam {
#ifndef NS_END
	#define NS_END }
#endif

NS_GLEAM

// $TODO: Move this to a more appropriate file.
enum class ComparisonFunc
{
	Never = 1,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always
};

enum class RendererType
{
	Direct3D11 = 0,
	Direct3D12,
	Vulkan,
	Metal,

	Count
};

static constexpr RendererType GetRendererType(void)
{
#ifdef GLEAM_USE_D3D11
	return RendererType::Direct3D11;
#elif defined(GLEAM_USE_D3D12)
	return RendererType::Direct3D12;
#elif defined(GLEAM_USE_VULKAN)
	return RendererType::Vulkan;
#elif defined(GLEAM_USE_METAL)
	return RendererType::Metal;
#endif
}

NS_END
