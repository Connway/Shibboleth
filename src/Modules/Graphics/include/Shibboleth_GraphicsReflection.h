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

#pragma once

#include <Shibboleth_Reflection.h>
#include <Gleam_ISamplerState.h>
#include <Gleam_IRasterState.h>
#include <Gleam_IWindow.h>
#include <Gleam_IShader.h>

SHIB_REFLECTION_DECLARE(Gleam::ISamplerState::Filter)
SHIB_REFLECTION_DECLARE(Gleam::ISamplerState::Wrap)
SHIB_REFLECTION_DECLARE(Gleam::IShader::Type)
SHIB_REFLECTION_DECLARE(Gleam::IWindow::WindowMode)
SHIB_REFLECTION_DECLARE(Gleam::ComparisonFunc)
SHIB_REFLECTION_DECLARE(Gleam::StencilOp)

SHIB_REFLECTION_DECLARE(Gleam::ISamplerState::Settings)
SHIB_REFLECTION_DECLARE(Gleam::IRasterState::Settings)
