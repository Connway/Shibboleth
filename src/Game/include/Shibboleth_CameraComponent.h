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

#include "Shibboleth_IComponent.h"
#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_String.h>

NS_SHIBBOLETH

class IApp;

class CameraComponent : public IComponent
{
public:
	CameraComponent(IApp& app);
	~CameraComponent(void);

	bool load(const Gaff::JSON& json);

	void* rawRequestInterface(unsigned int class_id) const;

	const AString& getRenderTargetName(void) const;

private:
	template <unsigned int bit> bool GetFlag(void) const;
	template <unsigned int bit> void SetFlag(bool value);

	AString _render_target_name;
	char _option_flags;

	IApp& _app;

	REF_DEF_SHIB(CameraComponent);
};

NS_END
