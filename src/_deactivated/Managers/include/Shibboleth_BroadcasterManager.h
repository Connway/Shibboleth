/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Shibboleth_IBroadcasterManager.h"
#include <Shibboleth_IUpdateQuery.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_Map.h>
#include <Gaff_ReadWriteSpinLock.h>

NS_SHIBBOLETH

class BroadcasterManager : public IManager, public IUpdateQuery, public IBroadcasterManager
{
public:
	BroadcasterManager(void);
	~BroadcasterManager(void);

	const char* getName(void) const override;

	void getUpdateEntries(Array<UpdateEntry>& entries) override;

	MessageBroadcaster* getBroadcaster(unsigned int object_id, bool create_if_doesnt_exist) override;
	void clear(void) override;

private:
	Map<unsigned int, MessageBroadcaster*> _object_broadcasters;
	Gaff::ReadWriteSpinLock _broadcaster_lock;

	void update(double, void*);

	SHIB_REF_DEF(BroadcasterManager);
	REF_DEF_REQ;
};

NS_END