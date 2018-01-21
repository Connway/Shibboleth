/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

	const ICommandList& operator=(const ICommandList& rhs) override;
	const ICommandList& operator=(ICommandList&& rhs) override;

	bool operator==(const ICommandList& rhs) const override;
	bool operator!=(const ICommandList& rhs) const override;

	RendererType getRendererType(void) const override;

	void append(const CommandListGL& command_list);
	void execute(void);
	void clear(void);

	template <class ReturnType, class... Args>
	void addCommand(const Gaff::CachedFunction<ReturnType, Args...>& function)
	{
		// For some reason using the GAFF_ALLOCT macro is confusing the compiler, even though it expands to the same as the line below it.
		//Gaff::IFunction<void>* func = GAFF_ALLOCT(Gaff::CachedFunction<ReturnType, Args...>, *GetAllocator(), function);
		Gaff::IFunction<void>* func = (*GetAllocator()).template allocT< Gaff::CachedFunction<ReturnType, Args...> >(__FILE__, __LINE__, function);
		Gaff::SharedPtr< Gaff::IFunction<void> > cmd_ptr(func);
		_commands.emplacePush(std::move(cmd_ptr));
	}

private:
	GleamArray<Gaff::SharedPtr< Gaff::IFunction<void> > > _commands;
};

NS_END
