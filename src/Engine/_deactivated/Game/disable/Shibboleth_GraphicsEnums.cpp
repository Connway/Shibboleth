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

#include "Shibboleth_GraphicsEnums.h"

NS_SHIBBOLETH

// Default values
SHIB_ENUM_REF_IMPL(DisplayTags)
.addValue("DT_1", DT_1)
.addValue("DT_2", DT_2)
.addValue("DT_3", DT_3)
.addValue("DT_4", DT_4)
.addValue("DT_5", DT_5)
.addValue("DT_6", DT_6)
.addValue("DT_7", DT_7)
.addValue("DT_8", DT_8)
.addValue("DT_9", DT_9)
.addValue("DT_10", DT_10)
.addValue("DT_11", DT_11)
.addValue("DT_12", DT_12)
.addValue("DT_13", DT_13)
.addValue("DT_14", DT_14)
.addValue("DT_15", DT_15)
.addValue("DT_16", DT_16)
;

SHIB_ENUM_REF_IMPL(RenderPasses)
.addValue("Opaque", RP_OPAQUE)
.addValue("Transparent", RP_TRANSPARENT)
;

SHIB_ENUM_REF_IMPL_EMBEDDED(Gleam_ISamplerState_Filter, Gleam::ISamplerState::FILTER)
.addValue("NEAREST NEAREST NEAREST", Gleam::ISamplerState::FILTER_NEAREST_NEAREST_NEAREST)
.addValue("NEAREST LINEAR NEAREST", Gleam::ISamplerState::FILTER_NEAREST_LINEAR_NEAREST)
.addValue("NEAREST LINEAR LINEAR", Gleam::ISamplerState::FILTER_NEAREST_LINEAR_LINEAR)
.addValue("LINEAR NEAREST NEAREST", Gleam::ISamplerState::FILTER_LINEAR_NEAREST_NEAREST)
.addValue("LINEAR NEAREST LINEAR", Gleam::ISamplerState::FILTER_LINEAR_NEAREST_LINEAR)
.addValue("LINEAR LINEAR NEAREST", Gleam::ISamplerState::FILTER_LINEAR_LINEAR_NEAREST)
.addValue("LINEAR LINEAR LINEAR", Gleam::ISamplerState::FILTER_LINEAR_LINEAR_LINEAR)
.addValue("ANISOTROPIC", Gleam::ISamplerState::FILTER_ANISOTROPIC)
;

SHIB_ENUM_REF_IMPL_EMBEDDED(Gleam_ISamplerState_Wrap, Gleam::ISamplerState::WRAP)
.addValue("REPEAT", Gleam::ISamplerState::WRAP_REPEAT)
.addValue("MIRROR", Gleam::ISamplerState::WRAP_MIRROR)
.addValue("CLAMP", Gleam::ISamplerState::WRAP_CLAMP)
.addValue("BORDER", Gleam::ISamplerState::WRAP_BORDER)
;

SHIB_ENUM_REF_IMPL_EMBEDDED(Gleam_ITexture_Format, Gleam::ITexture::FORMAT)
.addValue("R_8_UNORM", Gleam::ITexture::R_8_UNORM)
.addValue("R_16_UNORM", Gleam::ITexture::R_16_UNORM)
.addValue("RG_8_UNORM", Gleam::ITexture::RG_8_UNORM)
.addValue("RG_16_UNORM", Gleam::ITexture::RG_16_UNORM)
.addValue("RGBA_8_UNORM", Gleam::ITexture::RGBA_8_UNORM)
.addValue("RGBA_16_UNORM", Gleam::ITexture::RGBA_16_UNORM)
.addValue("R_8_SNORM", Gleam::ITexture::R_8_UNORM)
.addValue("R_16_SNORM", Gleam::ITexture::R_16_SNORM)
.addValue("RG_8_SNORM", Gleam::ITexture::RG_8_SNORM)
.addValue("RG_16_SNORM", Gleam::ITexture::RG_16_SNORM)
.addValue("RGBA_8_SNORM", Gleam::ITexture::RGBA_8_SNORM)
.addValue("R_8_I", Gleam::ITexture::R_8_I)
.addValue("R_16_I", Gleam::ITexture::R_16_I)
.addValue("R_32_I", Gleam::ITexture::R_32_I)
.addValue("RG_8_I", Gleam::ITexture::RG_8_I)
.addValue("RG_16_I", Gleam::ITexture::RG_16_I)
.addValue("RG_32_I", Gleam::ITexture::RG_32_I)
.addValue("RGB_32_I", Gleam::ITexture::RGB_32_I)
.addValue("RGBA_8_I", Gleam::ITexture::RGBA_8_I)
.addValue("RGBA_16_I", Gleam::ITexture::RGBA_16_I)
.addValue("RGBA_32_I", Gleam::ITexture::RGBA_32_I)
.addValue("R_8_UI", Gleam::ITexture::R_8_UI)
.addValue("R_16_UI", Gleam::ITexture::R_16_UI)
.addValue("R_32_UI", Gleam::ITexture::R_32_UI)
.addValue("RG_8_UI", Gleam::ITexture::RG_8_UI)
.addValue("RG_16_UI", Gleam::ITexture::RG_16_UI)
.addValue("RG_32_UI", Gleam::ITexture::RG_32_UI)
.addValue("RGB_32_UI", Gleam::ITexture::RGB_32_UI)
.addValue("RGBA_8_UI", Gleam::ITexture::RGBA_8_UI)
.addValue("RGBA_16_UI", Gleam::ITexture::RGBA_16_UI)
.addValue("RGBA_32_UI", Gleam::ITexture::RGBA_32_UI)
.addValue("R_16_F", Gleam::ITexture::R_16_F)
.addValue("R_32_F", Gleam::ITexture::R_32_F)
.addValue("RG_16_F", Gleam::ITexture::RG_16_F)
.addValue("RG_32_F", Gleam::ITexture::RG_32_F)
.addValue("RGB_16_F", Gleam::ITexture::RGB_16_F)
.addValue("RGB_32_F", Gleam::ITexture::RGB_32_F)
.addValue("RGBA_16_F", Gleam::ITexture::RGBA_16_F)
.addValue("RGBA_32_F", Gleam::ITexture::RGBA_32_F)
.addValue("RGB_11_11_10_F", Gleam::ITexture::RGB_11_11_10_F)
.addValue("RGBE_9_9_9_5", Gleam::ITexture::RGBE_9_9_9_5)
.addValue("RGBA_10_10_10_2_UNORM", Gleam::ITexture::RGBA_10_10_10_2_UNORM)
.addValue("RGBA_10_10_10_2_UI", Gleam::ITexture::RGBA_10_10_10_2_UI)
.addValue("SRGBA_8_UNORM", Gleam::ITexture::SRGBA_8_UNORM)
.addValue("DEPTH_16_UNORM", Gleam::ITexture::DEPTH_16_UNORM)
.addValue("DEPTH_32_F", Gleam::ITexture::DEPTH_32_F)
.addValue("DEPTH_STENCIL_24_8_UNORM_UI", Gleam::ITexture::DEPTH_STENCIL_24_8_UNORM_UI)
.addValue("DEPTH_STENCIL_32_8_F", Gleam::ITexture::DEPTH_STENCIL_32_8_F)
;

SHIB_ENUM_REF_IMPL_EMBEDDED(Gleam_IShader_Type, Gleam::IShader::SHADER_TYPE)
.addValue("Vertex", Gleam::IShader::SHADER_VERTEX)
.addValue("Pixel", Gleam::IShader::SHADER_PIXEL)
.addValue("Domain", Gleam::IShader::SHADER_DOMAIN)
.addValue("Geometry", Gleam::IShader::SHADER_GEOMETRY)
.addValue("Hull", Gleam::IShader::SHADER_HULL)
;

SHIB_ENUM_REF_IMPL_EMBEDDED(BUFFER_TYPE, Gleam::IBuffer::BUFFER_TYPE)
.addValue("Vertex Data", Gleam::IBuffer::VERTEX_DATA)
.addValue("Index Data", Gleam::IBuffer::INDEX_DATA)
.addValue("Shader Data", Gleam::IBuffer::SHADER_DATA)
.addValue("Structured Data", Gleam::IBuffer::STRUCTURED_DATA)
;

SHIB_ENUM_REF_IMPL_EMBEDDED(MAP_TYPE, Gleam::IBuffer::MAP_TYPE)
.addValue("None", Gleam::IBuffer::NONE)
.addValue("Read", Gleam::IBuffer::READ)
.addValue("Write", Gleam::IBuffer::WRITE)
.addValue("Read/Write", Gleam::IBuffer::READ_WRITE)
.addValue("Write No Overwrite", Gleam::IBuffer::WRITE_NO_OVERWRITE)
;

NS_END
