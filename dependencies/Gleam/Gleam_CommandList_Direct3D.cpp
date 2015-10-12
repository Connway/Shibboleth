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

#include "Gleam_CommandList_Direct3D.h"
#include <Gaff_IncludeAssert.h>

NS_GLEAM

CommandListD3D::CommandListD3D(const CommandListD3D& command_list):
	_command_list(command_list._command_list)
{
}

CommandListD3D::CommandListD3D(CommandListD3D&& command_list):
	_command_list(std::move(command_list._command_list))
{
}

CommandListD3D::CommandListD3D(void)
{
}

CommandListD3D::~CommandListD3D(void)
{
	SAFERELEASE(_command_list);
}

const ICommandList& CommandListD3D::operator=(const ICommandList& rhs)
{
	assert(rhs.isD3D());
	_command_list = reinterpret_cast<const CommandListD3D&>(rhs)._command_list;
	return *this;
}

const ICommandList& CommandListD3D::operator=(ICommandList&& rhs)
{
	assert(rhs.isD3D());
	_command_list = std::move(reinterpret_cast<CommandListD3D&>(rhs)._command_list);
	return *this;
}

bool CommandListD3D::operator==(const ICommandList& rhs) const
{
	assert(rhs.isD3D());
	return _command_list == reinterpret_cast<const CommandListD3D&>(rhs)._command_list;
}

bool CommandListD3D::operator!=(const ICommandList& rhs) const
{
	assert(rhs.isD3D());
	return _command_list != reinterpret_cast<const CommandListD3D&>(rhs)._command_list;
}

bool CommandListD3D::isD3D(void) const
{
	return true;
}

void CommandListD3D::setCommandList(ID3D11CommandList* command_list)
{
	SAFERELEASE(_command_list);
	_command_list = command_list;
}

ID3D11CommandList* CommandListD3D::getCommandList(void)
{
	return _command_list.get();
}

NS_END
