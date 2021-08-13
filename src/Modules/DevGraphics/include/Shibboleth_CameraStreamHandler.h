/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include <Shibboleth_IDevWebHandler.h>
#include <Shibboleth_Reflection.h>

NS_SHIBBOLETH

class ECSManager;

class CameraStreamHandler final : public IDevWebHandler, public Gaff::IReflectionObject
{
public:
	CameraStreamHandler(void);

	bool init(void) override;
	void destroy(void) override;

	bool handleGet(CivetServer* server, mg_connection* conn) override;
	bool handlePut(CivetServer* server, mg_connection* conn) override;

	SHIB_REFLECTION_CLASS_DECLARE(CameraStreamHandler);

private:
	void* _encoder = nullptr;
	bool _active = false;
};

NS_END

SHIB_REFLECTION_DECLARE(CameraStreamHandler)
