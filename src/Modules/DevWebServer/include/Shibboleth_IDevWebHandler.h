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

#include <Shibboleth_Defines.h>
#include "Shibboleth_IncludeCivetWeb.h"

NS_SHIBBOLETH

class IDevWebHandler : public CivetHandler
{
public:
	virtual bool init(void);
	virtual void update(void) {}
	virtual void destroy(void) {}

	virtual void handleConnectionClosed(const mg_connection* conn);

	bool handleGet(CivetServer* server, mg_connection* conn) override;
	bool handlePost(CivetServer* server, mg_connection* conn) override;
	bool handleHead(CivetServer* server, mg_connection* conn) override;
	bool handlePut(CivetServer* server, mg_connection* conn) override;
	bool handleDelete(CivetServer* server, mg_connection* conn) override;
	bool handleOptions(CivetServer* server, mg_connection* conn) override;
	bool handlePatch(CivetServer* server, mg_connection* conn) override;
};

NS_END
