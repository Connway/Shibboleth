/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Gleam_IShader.h"
#include "Gleam_HashMap.h"
#include "Gleam_String.h"
#include "Gleam_Array.h"

NS_GLEAM

class IRenderDevice;
class IProgram;

class ShaderProgramManager
{
public:
	ShaderProgramManager(void);
	~ShaderProgramManager(void);

	void destroy(void);

	IProgram* createProgram(const GleamAString& name = "");
	IShader* createShader(IRenderDevice& rd, const GleamGString& file_path, IShader::SHADER_TYPE shader_type, const GleamAString& name = "");

	int addProgram(IProgram* program, const GleamAString& name = "");
	int addShader(IShader* shader, const GleamAString& name = "");

	bool removeProgram(const IProgram* program);
	void removeProgram(int index);
	bool removeShader(const IShader* shader);
	void removeShader(int index);

	INLINE int getProgramIndex(const GleamAString& name) const;
	INLINE int getShaderIndex(const GleamAString& name) const;

	INLINE int getIndex(const IProgram* program) const;
	INLINE int getIndex(const IShader* shader) const;

	INLINE const IProgram* getProgram(int index) const;
	INLINE IProgram* getProgram(int index);

	INLINE const IShader* getShader(int index) const;
	INLINE IShader* getShader(int index);

private:
	GleamHashMap<GleamAString, int> _program_map;
	GleamHashMap<GleamAString, int> _shader_map;
	GleamArray<IProgram*> _programs;
	GleamArray<IShader*> _shaders;

	void addProgramEntry(const GleamAString& name);
	void addShaderEntry(const GleamAString& name);
};

NS_END
