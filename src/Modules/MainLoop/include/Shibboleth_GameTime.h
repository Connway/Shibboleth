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

#include "Shibboleth_ISystem.h"
#include <Shibboleth_Reflection.h>
#include <Shibboleth_IManager.h>
#include <EASTL/chrono.h>

NS_SHIBBOLETH

struct Time final
{
	double total = 0.0;
	double delta = 0.0;

	float GetTotalFloat(void) const { return static_cast<float>(total); }
	float GetDeltaFloat(void) const { return static_cast<float>(delta); }
};

class GameTimeManager final : public IManager
{
public:
	void update(void);
	void reset(void);

	void setGameTimeScale(double scale);
	double getGameTimeScale(void) const;

	const Time& getRealTime(void) const;
	const Time& getGameTime(void) const;

private:
	eastl::chrono::time_point<eastl::chrono::high_resolution_clock> _start;
	eastl::chrono::time_point<eastl::chrono::high_resolution_clock> _end;

	Time _real_time;
	Time _game_time;

	double _game_time_scale = 1.0;

	SHIB_REFLECTION_CLASS_DECLARE(GameTimeManager);
};

class GameTimeSystem final : public ISystem
{
public:
	bool init(void) override;
	void update() override;

private:
	GameTimeManager* _manager = nullptr;

	SHIB_REFLECTION_CLASS_DECLARE(GameTimeSystem);
};

NS_END

SHIB_REFLECTION_DECLARE(GameTimeManager)
SHIB_REFLECTION_DECLARE(GameTimeSystem)
