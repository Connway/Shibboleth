/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

SHIB_REFLECTION_EXTERNAL_DEFINE(Gleam::IRasterState::RasterStateSettings)
SHIB_REFLECTION_BUILDER_BEGIN(Gleam::IRasterState::RasterStateSettings)
	.var("slope_scale_depth_bias", &Gleam::IRasterState::RasterStateSettings::slope_scale_depth_bias)
	.var("depth_bias_clamp", &Gleam::IRasterState::RasterStateSettings::depth_bias_clamp)
	.var("depth_bias", &Gleam::IRasterState::RasterStateSettings::depth_bias)
	.var("depth_clip_enabled", &Gleam::IRasterState::RasterStateSettings::depth_clip_enabled)
	.var("front_face_counter_clockwise", &Gleam::IRasterState::RasterStateSettings::front_face_counter_clockwise)
	.var("scissor_enabled", &Gleam::IRasterState::RasterStateSettings::scissor_enabled)
	.var("two_sided", &Gleam::IRasterState::RasterStateSettings::two_sided)
	.var("wireframe", &Gleam::IRasterState::RasterStateSettings::wireframe)
SHIB_REFLECTION_BUILDER_END(Gleam::IRasterState::RasterStateSettings)

SHIB_ENUM_REFLECTION_DEFINE(Gleam::IShader::ShaderType)
SHIB_ENUM_REFLECTION_BEGIN(Gleam::IShader::ShaderType)
	.entry("Vertex", Gleam::IShader::SHADER_VERTEX)
	.entry("Pixel", Gleam::IShader::SHADER_PIXEL)
	.entry("Domain", Gleam::IShader::SHADER_DOMAIN)
	.entry("Geometry", Gleam::IShader::SHADER_GEOMETRY)
	.entry("Hull", Gleam::IShader::SHADER_HULL)
	.entry("Compute", Gleam::IShader::SHADER_COMPUTE)
SHIB_ENUM_REFLECTION_END(Gleam::IShader::ShaderType)

SHIB_ENUM_REFLECTION_DEFINE(Gleam::IWindow::WindowMode)
SHIB_ENUM_REFLECTION_BEGIN(Gleam::IWindow::WindowMode)
	.entry("Fullscreen", Gleam::IWindow::WM_FULLSCREEN)
	.entry("Windowed", Gleam::IWindow::WM_WINDOWED)
	.entry("Borderless Windowed", Gleam::IWindow::WM_BORDERLESS_WINDOWED)
SHIB_ENUM_REFLECTION_END(Gleam::IWindow::WindowMode)
