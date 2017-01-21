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

/*! \file */

#pragma once

#include "Gaff_NetworkDefines.h"
#include "Gaff_Function.h"

struct _ENetPeer;

NS_GAFF

/*!
	Flags that are used to determine how a packet is sent over the wire.
*/
enum PacketFlags
{
	PACKET_RELIABLE = (1 << 0), //!< Ensures the packet will be received and be received in order.
	PACKET_UNSEQUENCED = (1 << 1), //!< Packet will not be sequenced with other packets. Not supported for reliable packets.
	PACKET_NO_ALLOCATE = (1 << 2), //!< Packet will not allocate data and user must supply it instead.
	PACKET_UNRELIABLE_FRAGMENT = (1 << 3) //!< Packet will be fragmented using unreliable (instead of reliable) send if it exceeds the MTU.
};

class Host;

/*!
	\brief Class that represents a connection to another peer.
*/
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

	INLINE PeerIDType getID(void) const;
	INLINE unsigned int getHost(void) const;
	INLINE unsigned short getPort(void) const;

	INLINE void* getUserData(void) const;
	INLINE void setUserData(void* data);

	INLINE void setThrottle(unsigned int interval, unsigned int acceleration, unsigned int deceleration);
	INLINE void setTimeout(unsigned int limit, unsigned int min, unsigned int max);
	INLINE void setPingInterval(unsigned int interval);

	INLINE void disconnect(unsigned int data = 0);
	INLINE void disconnectNow(unsigned int data = 0);
	INLINE void disconnectLater(unsigned int data = 0);

	INLINE void ping(void);

	INLINE bool send(unsigned char channel, void* data, size_t data_size, unsigned int packet_flags = PACKET_RELIABLE);

	INLINE bool valid(void) const;

	INLINE bool operator==(const Connection& rhs) const;
	INLINE bool operator!=(const Connection& rhs) const;

	const Connection& operator=(const Connection& rhs);

	INLINE operator bool(void) const;

private:
	_ENetPeer* _peer;
	bool _cleanup;

	Connection(_ENetPeer* peer);

	void connect(Host& host, const char* address, unsigned short port, size_t channels, unsigned int data);

	friend class Host;
};

NS_END
