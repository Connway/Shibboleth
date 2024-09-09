/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Shibboleth_GameTime.h"
#include <Attributes/Shibboleth_EngineAttributesCommon.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_Math.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::GameTimeManager)
	.template base<Shibboleth::IManager>()
	.template ctor<>()

	.var("game_time_scale", &Shibboleth::GameTimeManager::_game_time_scale)
	.var("real_time", &Shibboleth::GameTimeManager::_real_time)
	.var("game_time", &Shibboleth::GameTimeManager::_game_time)
SHIB_REFLECTION_DEFINE_END(Shibboleth::GameTimeManager)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::Time)
	.var("total", &Shibboleth::Time::total)
	.var("delta", &Shibboleth::Time::delta)
SHIB_REFLECTION_DEFINE_END(Shibboleth::Time)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(GameTimeManager)

constexpr double k_max_delta_game_time = 0.75;

void GameTimeManager::update(void)
{
	using DoubleSeconds = eastl::chrono::duration<double>;

	_end = eastl::chrono::high_resolution_clock::now();
	const DoubleSeconds delta = _end - _start;
	_start = _end;

	// Don't apply the first frame so we can get a true delta from here on out.
	if (_first_frame) {
		_first_frame = false;
		return;
	}

	_real_time.delta = delta.count();
	_real_time.total += _real_time.delta;

	_game_time.delta = Gaff::Min(_real_time.delta, k_max_delta_game_time) * _game_time_scale;
	_game_time.total += _game_time.delta;
}

void GameTimeManager::reset(void)
{
	_start = _end = eastl::chrono::high_resolution_clock::now();
}

void GameTimeManager::setGameTimeScale(double scale)
{
	_game_time_scale = Gaff::Max(0.0, scale);
}

double GameTimeManager::getGameTimeScale(void) const
{
	return _game_time_scale;
}

const Time& GameTimeManager::getRealTime(void) const
{
	return _real_time;
}

const Time& GameTimeManager::getGameTime(void) const
{
	return _game_time;
}

NS_END
