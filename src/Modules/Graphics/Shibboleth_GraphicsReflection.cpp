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

#include "Shibboleth_GraphicsReflection.h"
#include <Shibboleth_Math.h>

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::ISamplerState::Filter)
	.entry("NearestNearestNearest", Gleam::ISamplerState::Filter::NearestNearestNearest)
	.entry("NearestNearestLinear", Gleam::ISamplerState::Filter::NearestNearestLinear)
	.entry("NearestLinearNearest", Gleam::ISamplerState::Filter::NearestLinearNearest)
	.entry("NearestLinearLinear", Gleam::ISamplerState::Filter::NearestLinearLinear)
	.entry("LinearNearestNearest", Gleam::ISamplerState::Filter::LinearNearestNearest)
	.entry("LinearNearestLinear", Gleam::ISamplerState::Filter::LinearNearestLinear)
	.entry("LinearLinearNearest", Gleam::ISamplerState::Filter::LinearLinearNearest)
	.entry("LinearLinearLinear", Gleam::ISamplerState::Filter::LinearLinearLinear)
	.entry("Anisotropic", Gleam::ISamplerState::Filter::Anisotropic)
SHIB_REFLECTION_DEFINE_END(Gleam::ISamplerState::Filter)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::ISamplerState::Wrap)
	.entry("Repeat", Gleam::ISamplerState::Wrap::Repeat)
	.entry("Mirror", Gleam::ISamplerState::Wrap::Mirror)
	.entry("Clamp", Gleam::ISamplerState::Wrap::Clamp)
	.entry("Border", Gleam::ISamplerState::Wrap::Border)
SHIB_REFLECTION_DEFINE_END(Gleam::ISamplerState::Wrap)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::IShader::Type)
	.entry("Vertex", Gleam::IShader::Type::Vertex)
	.entry("Pixel", Gleam::IShader::Type::Pixel)
	.entry("Domain", Gleam::IShader::Type::Domain)
	.entry("Geometry", Gleam::IShader::Type::Geometry)
	.entry("Hull", Gleam::IShader::Type::Hull)
	.entry("Compute", Gleam::IShader::Type::Compute)
SHIB_REFLECTION_DEFINE_END(Gleam::IShader::Type)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::ComparisonFunc)
	.entry("Never", Gleam::ComparisonFunc::Never)
	.entry("Less", Gleam::ComparisonFunc::Less)
	.entry("Equal", Gleam::ComparisonFunc::Equal)
	.entry("LessEqual", Gleam::ComparisonFunc::LessEqual)
	.entry("Greater", Gleam::ComparisonFunc::Greater)
	.entry("NotEqual", Gleam::ComparisonFunc::NotEqual)
	.entry("GreaterEqual", Gleam::ComparisonFunc::GreaterEqual)
	.entry("Always", Gleam::ComparisonFunc::Always)
	SHIB_REFLECTION_DEFINE_END(Gleam::ComparisonFunc)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::StencilOp)
	.entry("Keep", Gleam::StencilOp::Keep)
	.entry("Zero", Gleam::StencilOp::Zero)
	.entry("Replace", Gleam::StencilOp::Replace)
	.entry("IncrementClamp", Gleam::StencilOp::IncrementClamp)
	.entry("DecrementClamp", Gleam::StencilOp::DecrementClanp)
	.entry("Invert", Gleam::StencilOp::Invert)
	.entry("IncrementWrap", Gleam::StencilOp::IncrementWrap)
	.entry("DecrementWrap", Gleam::StencilOp::DecrementWrap)
SHIB_REFLECTION_DEFINE_END(Gleam::StencilOp)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::ISamplerState::Settings)
	.var("filter", &Gleam::ISamplerState::Settings::filter)
	.var("wrap_u", &Gleam::ISamplerState::Settings::wrap_u)
	.var("wrap_v", &Gleam::ISamplerState::Settings::wrap_v)
	.var("wrap_w", &Gleam::ISamplerState::Settings::wrap_w)
	.var("min_lod", &Gleam::ISamplerState::Settings::min_lod)
	.var("max_lod", &Gleam::ISamplerState::Settings::max_lod)
	.var("lod_bias", &Gleam::ISamplerState::Settings::lod_bias)
	.var("max_anisotropy", &Gleam::ISamplerState::Settings::max_anisotropy)
	.var("border_color", &Gleam::ISamplerState::Settings::border_color)
	.var("compare_func", &Gleam::ISamplerState::Settings::compare_func)
SHIB_REFLECTION_DEFINE_END(Gleam::ISamplerState::Settings)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::IRasterState::Settings)
	.var("slope_scale_depth_bias", &Gleam::IRasterState::Settings::slope_scale_depth_bias)
	.var("depth_bias_clamp", &Gleam::IRasterState::Settings::depth_bias_clamp)
	.var("depth_bias", &Gleam::IRasterState::Settings::depth_bias)
	.var("depth_clip_enabled", &Gleam::IRasterState::Settings::depth_clip_enabled)
	.var("front_face_counter_clockwise", &Gleam::IRasterState::Settings::front_face_counter_clockwise)
	.var("scissor_enabled", &Gleam::IRasterState::Settings::scissor_enabled)
	.var("two_sided", &Gleam::IRasterState::Settings::two_sided)
	.var("wireframe", &Gleam::IRasterState::Settings::wireframe)
SHIB_REFLECTION_DEFINE_END(Gleam::IRasterState::Settings)
