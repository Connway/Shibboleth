/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

enum class Error
{
	Success = 0,

	GenerateHeaders_FailedToGenerateModule = -1,
	GenerateHeaders_FailedToGenerateTool = -2,
	GenerateHeaders_FailedToGenerateStatic = -3,

	Preproc_FailedToPreprocModule = -4,
	Preproc_FailedToPreprocEngine = -5,
	Preproc_FailedToPreprocTool = -6,

	BuildProject_FailedToOpenVSDir = -7,
	BuildProject_FailedToReadVSDir = -8,
	BuildProject_NoProjectSpecified = -9,
	BuildProject_NoConfigSpecified = -10
};
