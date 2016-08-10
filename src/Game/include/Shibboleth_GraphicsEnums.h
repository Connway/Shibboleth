/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include <Shibboleth_ReflectionDefinitions.h>
#include <Gleam_ISamplerState.h>
#include <Gleam_ITexture.h>
#include <Gleam_IShader.h>
#include <Gleam_IBuffer.h>

NS_SHIBBOLETH

enum DisplayTags
{
	DT_ALL = 0xFFFF,
	DT_1 = FLAG_BIT(1),
	DT_2 = FLAG_BIT(2),
	DT_3 = FLAG_BIT(3),
	DT_4 = FLAG_BIT(4),
	DT_5 = FLAG_BIT(5),
	DT_6 = FLAG_BIT(6),
	DT_7 = FLAG_BIT(7),
	DT_8 = FLAG_BIT(8),
	DT_9 = FLAG_BIT(9),
	DT_10 = FLAG_BIT(10),
	DT_11 = FLAG_BIT(11),
	DT_12 = FLAG_BIT(12),
	DT_13 = FLAG_BIT(13),
	DT_14 = FLAG_BIT(14),
	DT_15 = FLAG_BIT(15),
	DT_16 = FLAG_BIT(16)
};

enum RenderPasses
{
	RP_OPAQUE = 0,
	RP_TRANSPARENT,
	RP_COUNT
};

SHIB_ENUM_REF_DEF(RenderPasses);
SHIB_ENUM_REF_DEF(DisplayTags);
SHIB_ENUM_REF_DEF_EMBEDDED(Gleam_ISamplerState_Filter, Gleam::ISamplerState::FILTER);
SHIB_ENUM_REF_DEF_EMBEDDED(Gleam_ISamplerState_Wrap, Gleam::ISamplerState::WRAP);
SHIB_ENUM_REF_DEF_EMBEDDED(Gleam_ITexture_Format, Gleam::ITexture::FORMAT);
SHIB_ENUM_REF_DEF_EMBEDDED(Gleam_IShader_Type, Gleam::IShader::SHADER_TYPE);
SHIB_ENUM_REF_DEF_EMBEDDED(BUFFER_TYPE, Gleam::IBuffer::BUFFER_TYPE);
SHIB_ENUM_REF_DEF_EMBEDDED(MAP_TYPE, Gleam::IBuffer::MAP_TYPE);

NS_END
