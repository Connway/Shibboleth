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

#include "Gleam_CommandList_OpenGL.h"

NS_GLEAM

CommandListGL::CommandListGL(const CommandListGL& command_list):
	_commands(command_list._commands)
{
}

CommandListGL::CommandListGL(CommandListGL&& command_list):
	_commands(std::move(command_list._commands))
{
}

CommandListGL::CommandListGL(void)
{
}

CommandListGL::~CommandListGL(void)
{
}

const ICommandList& CommandListGL::operator=(const ICommandList& rhs)
{
	assert(rhs.getRendererType() == RENDERER_OPENGL);
	_commands = reinterpret_cast<const CommandListGL&>(rhs)._commands;
	return *this;
}

const ICommandList& CommandListGL::operator=(ICommandList&& rhs)
{
	assert(rhs.getRendererType() == RENDERER_OPENGL);
	_commands = std::move(reinterpret_cast<const CommandListGL&>(rhs)._commands);
	return *this;
}

bool CommandListGL::operator==(const ICommandList& rhs) const
{
	assert(rhs.getRendererType() == RENDERER_OPENGL);
	return _commands == reinterpret_cast<const CommandListGL&>(rhs)._commands;
}

bool CommandListGL::operator!=(const ICommandList& rhs) const
{
	assert(rhs.getRendererType() == RENDERER_OPENGL);
	return _commands != reinterpret_cast<const CommandListGL&>(rhs)._commands;
}

RendererType CommandListGL::getRendererType(void) const
{
	return RENDERER_OPENGL;
}

void CommandListGL::append(const CommandListGL& command_list)
{
	_commands.reserve(_commands.size() + command_list._commands.size());

	for (auto it = command_list._commands.begin(); it != command_list._commands.end(); ++it) {
		_commands.emplacePush(*it);
	}
}

void CommandListGL::execute(void)
{
	for (auto it = _commands.begin(); it != _commands.end(); ++it) {
		(*it)->call();
	}
}

void CommandListGL::clear(void)
{
	_commands.clear();
}

NS_END
