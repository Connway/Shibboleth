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

#include "Shibboleth_PlayerManager.h"
#include "Shibboleth_LocalPlayer.h"

namespace
{
	static Shibboleth::ProxyAllocator g_allocator("Player");
}


SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::PlayerManager)
	.template base<Shibboleth::IManager>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::PlayerManager)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(PlayerManager)

template <>
int32_t PlayerManager::getNumPlayersT<LocalPlayer>(void) const
{
	return getNumLocalPlayers();
}

template <>
const LocalPlayer& PlayerManager::getPlayerT<LocalPlayer>(int32_t index) const
{
	return getLocalPlayer(index);
}

template <>
LocalPlayer& PlayerManager::getPlayerT<LocalPlayer>(int32_t index)
{
	return getLocalPlayer(index);
}

template <>
int32_t PlayerManager::getNumPlayersT<Player>(void) const
{
	return getNumPlayers();
}

template <>
const Player& PlayerManager::getPlayerT<Player>(int32_t index) const
{
	return getPlayer(index);
}

template <>
Player& PlayerManager::getPlayerT<Player>(int32_t index)
{
	return getPlayer(index);
}

bool PlayerManager::initAllModulesLoaded(void)
{
	addLocalPlayer();
	return true;
}

const Player& PlayerManager::getPlayer(int32_t index) const
{
	return const_cast<PlayerManager*>(this)->getPlayer(index);
}

Player& PlayerManager::getPlayer(int32_t index)
{
	GAFF_ASSERT(_players.validIndex(index));
	return *_players[index];
}

int32_t PlayerManager::getNumPlayers(void) const
{
	return _players.getValidSize();
}

void PlayerManager::removePlayer(Player& player)
{
	const int32_t index = _players.find(&player, [](const UniquePtr<Player>& lhs, const Player* rhs) -> bool { return lhs.get() == rhs; })		;
	removePlayer(index);
}

void PlayerManager::removePlayer(int32_t index)
{
	const Player* const player = _players[index].get();
	// Since we're not dereferencing, this should be safe, even if player is not a LocalPlayer.
	const int32_t local_index = _local_players.find(static_cast<const LocalPlayer*>(player));

	if (local_index != -1) {
		_local_players.removeAt(local_index);
	}

	_players.removeAt(index);
}

const LocalPlayer& PlayerManager::getLocalPlayer(int32_t index) const
{
	return const_cast<PlayerManager*>(this)->getLocalPlayer(index);
}

LocalPlayer& PlayerManager::getLocalPlayer(int32_t index)
{
	GAFF_ASSERT(index >= 0 && index < getNumLocalPlayers());
	return *_local_players[index];
}

int32_t PlayerManager::getLocalPlayerIndex(const LocalPlayer& player) const
{
	return _local_players.find(&player);
}

int32_t PlayerManager::getNumLocalPlayers(void) const
{
	return static_cast<int32_t>(_local_players.getValidSize());
}

LocalPlayer& PlayerManager::addLocalPlayer(void)
{
	LocalPlayer* const player = SHIB_ALLOCT(LocalPlayer, g_allocator);

	player->init();

	_local_players.emplace(player);
	_players.emplace(player);

	return *player;
}

NS_END
