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

#include "Gleam_ICommandList.h"
#include "Gleam_IncludeD3D11.h"
#include <Gaff_RefPtr.h>

NS_GLEAM

class CommandListD3D11 : public ICommandList
{
public:
	CommandListD3D11(const CommandListD3D11& command_list);
	CommandListD3D11(CommandListD3D11&& command_list);
	CommandListD3D11(void);
	~CommandListD3D11(void);

	const ICommandList& operator=(const ICommandList& rhs) override;
	const ICommandList& operator=(ICommandList&& rhs) override;

	bool operator==(const ICommandList& rhs) const override;
	bool operator!=(const ICommandList& rhs) const override;

	RendererType getRendererType(void) const override;
	bool isValid(void) const override;

	void setCommandList(ID3D11CommandList* command_list);
	ID3D11CommandList* getCommandList(void);

private:
	Gaff::COMRefPtr<ID3D11CommandList> _command_list;
};

NS_END
