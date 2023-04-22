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

#include "Gleam_CommandList_Direct3D11.h"
#include <Gaff_Assert.h>

NS_GLEAM

CommandListD3D11::CommandListD3D11(const CommandListD3D11& command_list):
	_command_list(command_list._command_list)
{
}

CommandListD3D11::CommandListD3D11(CommandListD3D11&& command_list):
	_command_list(std::move(command_list._command_list))
{
}

CommandListD3D11::CommandListD3D11(void)
{
}

CommandListD3D11::~CommandListD3D11(void)
{
	GAFF_COM_SAFE_RELEASE(_command_list);
}

const ICommandList& CommandListD3D11::operator=(const ICommandList& rhs)
{
	GAFF_ASSERT(rhs.getRendererType() == RendererType::Direct3D11);
	_command_list = static_cast<const CommandListD3D11&>(rhs)._command_list;
	return *this;
}

const ICommandList& CommandListD3D11::operator=(ICommandList&& rhs)
{
	GAFF_ASSERT(rhs.getRendererType() == RendererType::Direct3D11);
	_command_list = std::move(static_cast<CommandListD3D11&>(rhs)._command_list);
	return *this;
}

bool CommandListD3D11::operator==(const ICommandList& rhs) const
{
	GAFF_ASSERT(rhs.getRendererType() == RendererType::Direct3D11);
	return _command_list == static_cast<const CommandListD3D11&>(rhs)._command_list;
}

bool CommandListD3D11::operator!=(const ICommandList& rhs) const
{
	GAFF_ASSERT(rhs.getRendererType() == RendererType::Direct3D11);
	return _command_list != static_cast<const CommandListD3D11&>(rhs)._command_list;
}

RendererType CommandListD3D11::getRendererType(void) const
{
	return RendererType::Direct3D11;
}

bool CommandListD3D11::isValid(void) const
{
	return _command_list != nullptr;
}

void CommandListD3D11::setCommandList(ID3D11CommandList* command_list)
{
	GAFF_COM_SAFE_RELEASE(_command_list);
	_command_list = command_list;
}

ID3D11CommandList* CommandListD3D11::getCommandList(void)
{
	return _command_list.get();
}

NS_END
