/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Gaff_NetworkDefines.h"

struct _ENetPeer;

NS_GAFF

enum PacketFlags
{
	PACKET_RELIABLE = (1 << 0),
	PACKET_UNSEQUENCED = (1 << 1),
	PACKET_NO_ALLOCATE = (1 << 2),
	PACKET_UNRELIABLE_FRAGMENT = (1 << 3)
};

class Host;

class Connection
{
public:
	template <class T>
	bool sendT(unsigned char channel, T* data, unsigned int packet_flags = PACKET_RELIABLE)
	{
		GAFF_ASSERT(_peer);
		return send(channel, data, sizeof(T), packet_flags);
	}

	Connection(const Connection& rhs);
	Connection(void);
	~Connection(void);

	void destroy(void);

	PeerIDType getID(void) const;
	unsigned int getHost(void) const;
	unsigned short getPort(void) const;

	void* getUserData(void) const;
	void setUserData(void* data);

	void setThrottle(unsigned int interval, unsigned int acceleration, unsigned int deceleration);
	void setTimeout(unsigned int limit, unsigned int min, unsigned int max);
	void setPingInterval(unsigned int interval);

	void disconnect(unsigned int data = 0);
	void disconnectNow(unsigned int data = 0);
	void disconnectLater(unsigned int data = 0);

	void ping(void);

	bool send(unsigned char channel, void* data, size_t data_size, unsigned int packet_flags = PACKET_RELIABLE);

	bool valid(void) const;

	bool operator==(const Connection& rhs) const;
	bool operator!=(const Connection& rhs) const;

	const Connection& operator=(const Connection& rhs);

	operator bool(void) const;

private:
	_ENetPeer* _peer;
	bool _cleanup;

	Connection(_ENetPeer* peer);

	void connect(Host& host, const char* address, unsigned short port, size_t channels, unsigned int data);

	friend class Host;
};

NS_END
