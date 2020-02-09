/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Shibboleth_GraphicsReflection.h"
#include <Shibboleth_Math.h>

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::ISamplerState::Filter)
	.entry("Nearest_Nearest_Nearest", Gleam::ISamplerState::FILTER_NEAREST_NEAREST_NEAREST)
	.entry("Nearest_Nearest_Linear", Gleam::ISamplerState::FILTER_NEAREST_NEAREST_LINEAR)
	.entry("Nearest_Linear_Nearest", Gleam::ISamplerState::FILTER_NEAREST_LINEAR_NEAREST)
	.entry("Nearest_Linear_Linear", Gleam::ISamplerState::FILTER_NEAREST_LINEAR_LINEAR)
	.entry("Linear_Nearest_Nearest", Gleam::ISamplerState::FILTER_LINEAR_NEAREST_NEAREST)
	.entry("Linear_Nearest_Linear", Gleam::ISamplerState::FILTER_LINEAR_NEAREST_LINEAR)
	.entry("Linear_Linear_Nearest", Gleam::ISamplerState::FILTER_LINEAR_LINEAR_NEAREST)
	.entry("Linear_Linear_Linear", Gleam::ISamplerState::FILTER_LINEAR_LINEAR_LINEAR)
	.entry("Anisotropic", Gleam::ISamplerState::FILTER_ANISOTROPIC)
SHIB_REFLECTION_DEFINE_END(Gleam::ISamplerState::Filter)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::ISamplerState::Wrap)
	.entry("Repeat", Gleam::ISamplerState::WRAP_REPEAT)
	.entry("Mirror", Gleam::ISamplerState::WRAP_MIRROR)
	.entry("Clamp", Gleam::ISamplerState::WRAP_CLAMP)
	.entry("Border", Gleam::ISamplerState::WRAP_BORDER)
SHIB_REFLECTION_DEFINE_END(Gleam::ISamplerState::Wrap)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::IShader::ShaderType)
	.entry("Vertex", Gleam::IShader::SHADER_VERTEX)
	.entry("Pixel", Gleam::IShader::SHADER_PIXEL)
	.entry("Domain", Gleam::IShader::SHADER_DOMAIN)
	.entry("Geometry", Gleam::IShader::SHADER_GEOMETRY)
	.entry("Hull", Gleam::IShader::SHADER_HULL)
	.entry("Compute", Gleam::IShader::SHADER_COMPUTE)
SHIB_REFLECTION_DEFINE_END(Gleam::IShader::ShaderType)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::IWindow::WindowMode)
	.entry("Fullscreen", Gleam::IWindow::WindowMode::Fullscreen)
	.entry("Windowed", Gleam::IWindow::WindowMode::Windowed)
	.entry("Borderless Windowed", Gleam::IWindow::WindowMode::BorderlessWindowed)
SHIB_REFLECTION_DEFINE_END(Gleam::IWindow::WindowMode)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::ComparisonFunc)
	.entry("Never", Gleam::ComparisonFunc::NEVER)
	.entry("Less", Gleam::ComparisonFunc::LESS)
	.entry("Equal", Gleam::ComparisonFunc::EQUAL)
	.entry("Less_Equal", Gleam::ComparisonFunc::LESS_EQUAL)
	.entry("Greater", Gleam::ComparisonFunc::GREATER)
	.entry("Not_Equal", Gleam::ComparisonFunc::NOT_EQUAL)
	.entry("Greater_Equal", Gleam::ComparisonFunc::GREATER_EQUAL)
SHIB_REFLECTION_DEFINE_END(Gleam::ComparisonFunc)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::StencilOp)
	.entry("Keep", Gleam::StencilOp::KEEP)
	.entry("Zero", Gleam::StencilOp::ZERO)
	.entry("Replace", Gleam::StencilOp::REPLACE)
	.entry("Increment_Clamp", Gleam::StencilOp::INCREMENT_CLAMP)
	.entry("Decrement_Clamp", Gleam::StencilOp::DECREMENT_CLAMP)
	.entry("Invert", Gleam::StencilOp::INVERT)
	.entry("Increment_Wrap", Gleam::StencilOp::INCREMENT_WRAP)
	.entry("Decrement_Wrap", Gleam::StencilOp::DECREMENT_WRAP)
SHIB_REFLECTION_DEFINE_END(Gleam::StencilOp)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::ISamplerState::SamplerSettings)
	.var("filter", &Gleam::ISamplerState::SamplerSettings::filter)
	.var("wrap_u", &Gleam::ISamplerState::SamplerSettings::wrap_u)
	.var("wrap_v", &Gleam::ISamplerState::SamplerSettings::wrap_v)
	.var("wrap_w", &Gleam::ISamplerState::SamplerSettings::wrap_w)
	.var("min_lod", &Gleam::ISamplerState::SamplerSettings::min_lod)
	.var("max_lod", &Gleam::ISamplerState::SamplerSettings::max_lod)
	.var("lod_bias", &Gleam::ISamplerState::SamplerSettings::lod_bias)
	.var("max_anisotropy", &Gleam::ISamplerState::SamplerSettings::max_anisotropy)
	.var("border_color", &Gleam::ISamplerState::SamplerSettings::border_color)
	.var("compare_func", &Gleam::ISamplerState::SamplerSettings::compare_func)
SHIB_REFLECTION_DEFINE_END(Gleam::ISamplerState::SamplerSettings)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::IRasterState::RasterSettings)
	.var("slope_scale_depth_bias", &Gleam::IRasterState::RasterSettings::slope_scale_depth_bias)
	.var("depth_bias_clamp", &Gleam::IRasterState::RasterSettings::depth_bias_clamp)
	.var("depth_bias", &Gleam::IRasterState::RasterSettings::depth_bias)
	.var("depth_clip_enabled", &Gleam::IRasterState::RasterSettings::depth_clip_enabled)
	.var("front_face_counter_clockwise", &Gleam::IRasterState::RasterSettings::front_face_counter_clockwise)
	.var("scissor_enabled", &Gleam::IRasterState::RasterSettings::scissor_enabled)
	.var("two_sided", &Gleam::IRasterState::RasterSettings::two_sided)
	.var("wireframe", &Gleam::IRasterState::RasterSettings::wireframe)
SHIB_REFLECTION_DEFINE_END(Gleam::IRasterState::RasterSettings)
