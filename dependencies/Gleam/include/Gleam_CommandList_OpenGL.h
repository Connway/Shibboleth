/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include "Gleam_ICommandList.h"
#include "Gleam_Array.h"
#include <Gaff_CachedFunction.h>
#include <Gaff_SharedPtr.h>

NS_GLEAM

class CommandListGL : public ICommandList
{
public:
	CommandListGL(const CommandListGL& command_list);
	CommandListGL(CommandListGL&& command_list);
	CommandListGL(void);
	~CommandListGL(void);

	const ICommandList& operator=(const ICommandList& rhs);
	const ICommandList& operator=(ICommandList&& rhs);

	bool operator==(const ICommandList& rhs) const;
	bool operator!=(const ICommandList& rhs) const;

	bool isD3D(void) const;

	void append(const CommandListGL& command_list);
	void execute(void);
	INLINE void clear(void);

	template <class ReturnType, class... Args>
	void addCommand(const Gaff::CachedFunction<ReturnType, Args...>& function)
	{
		Gaff::SharedPtr< Gaff::IFunction<void> > cmd_ptr(GetAllocator()->template allocT< GLFuncWrapper<ReturnType, Args...> >(function));
		_commands.emplaceMovePush(Gaff::Move(cmd_ptr));
	}

private:
	GleamArray<Gaff::SharedPtr< Gaff::IFunction<void> > > _commands;
};

NS_END