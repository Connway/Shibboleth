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

#pragma once

#include "Shibboleth_Player.h"
#include <Containers/Shibboleth_SparseStack.h>
#include <Shibboleth_IManager.h>

NS_SHIBBOLETH

class LocalPlayer;

class PlayerManager final : public IManager
{
public:
	template <class T>
	int32_t getNumPlayersT(void) const;

	template <class T>
	const T& getPlayerT(int32_t index) const;

	template <class T>
	T& getPlayerT(int32_t index);

	bool initAllModulesLoaded(void) override;

	const Player& getPlayer(int32_t index) const;
	Player& getPlayer(int32_t index);
	int32_t getNumPlayers(void) const;
	void removePlayer(Player& player);
	void removePlayer(int32_t index);

	// $TODO: If networking is ever added, add remote/proxy player class.

	const LocalPlayer& getLocalPlayer(int32_t index) const;
	LocalPlayer& getLocalPlayer(int32_t index);
	int32_t getLocalPlayerIndex(const LocalPlayer& player) const;
	int32_t getNumLocalPlayers(void) const;
	LocalPlayer& addLocalPlayer(void);

private:
	SparseStack< UniquePtr<Player> > _players{ ProxyAllocator("Player") };
	SparseStack<LocalPlayer*> _local_players{ ProxyAllocator("Player") };

	SHIB_REFLECTION_CLASS_DECLARE(PlayerManager);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::PlayerManager)
