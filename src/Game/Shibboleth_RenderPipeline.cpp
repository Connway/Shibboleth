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

#include "Shibboleth_RenderPipeline.h"
#include <Shibboleth_TaskPoolTags.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

NS_SHIBBOLETH

RenderPipeline::RenderPipeline(void)
{
}

RenderPipeline::~RenderPipeline(void)
{
}

bool RenderPipeline::init(const Gaff::JSON& json, const Array<IRenderStageQuery::RenderStageEntry>& entries)
{
	Gaff::JSON name = json["name"];
	Gaff::JSON stages = json["stages"];

	if (!name.isString()) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Element at 'name' is not a string.\n");
		return false;
	}

	if (!stages.isArray()) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Element at 'stages' is not an array.\n");
		return false;
	}

	_name = name.getString();

	bool failed = stages.forEachInArray([&](size_t, const Gaff::JSON& value) -> bool
	{
		if (!value.isString()) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Value in 'stages' is not a string.\n");
			return true;
		}

		auto it = entries.linearSearch(value.getString(), [](const IRenderStageQuery::RenderStageEntry& lhs, const char* rhs) -> bool
		{
			return lhs.first == rhs;
		});

		if (it == entries.end()) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to find entry for render stage '%s'.\n", value.getString());
			return true;
		}

		_stages.emplacePush(it->second);
		return false;
	});

	return !failed;
}

const char* RenderPipeline::getName(void) const
{
	return _name.getBuffer();
}

void RenderPipeline::run(void)
{
	for (auto it = _stages.begin(); it != _stages.end(); ++it) {
		(*it)();
	}
}

NS_END
