/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Gaff_Connection.h"

#define CONNECTION_SPEED_UNLIMITED 0
#define CONNECTION_SPEED_512K 65536
#define CONNECTION_SPEED_256K 32768
#define CONNECTION_SPEED_128K 16384
#define CONNECTION_SPEED_56K 7168

#if defined(_WIN32) || defined(_WIN64)
	#define NETWORK_CALLBACK __cdecl //!< Calling convention for network callback functions
#else
	#define NETWORK_CALLBACK //!< Calling convention for network callback functions
#endif

struct _ENetHost;
struct _ENetPeer;

NS_GAFF

typedef void* (NETWORK_CALLBACK *NetworkAllocFunc)(size_t size);
typedef void (NETWORK_CALLBACK *NetworkFreeFunc)(void* memory);
typedef void (NETWORK_CALLBACK *NetworkNoMemoryFunc)(void);

enum NetworkEventType
{
	EVENT_NONE = 0,
	EVENT_CONNECTION,
	EVENT_DISCONNECT,
	EVENT_RECEIVED_PACKET
};

/*!
	\brief Data passed into the network callback function when an event occurs.
*/
struct NetworkCallbackData
{
	Gaff::Host* host; //!< The host that generated this event.
	Gaff::NetworkEventType event_type;
	Gaff::PeerIDType peer_id; //!< The ID of the peer that generated this event.
	unsigned char channel; //!< The channel the packet data was sent over (if applicable).
	unsigned int user_data; //!< User data sent with the event.
	void* data; //!< Packet data (if applicable).
	size_t data_size; //!< Packet data size (if applicable).
};

typedef IFunction<void, const NetworkCallbackData&> NetworkEventCallback;

bool NetworkInit(
	NetworkAllocFunc alloc_func = nullptr,
	NetworkFreeFunc free_func = nullptr,
	NetworkNoMemoryFunc no_mem_func = nullptr
);

void NetworkDestroy(void);

class Host
{
public:
	template <class T>
	void broadcastT(unsigned char channel, T* data, unsigned int packet_flags = PACKET_RELIABLE)
	{
		assert(_host);
		broadcast(channel, data, sizeof(T), packet_flags);
	}

	Host(const Host& rhs);
	Host(void);
	~Host(void);

	bool initServer(unsigned short port, const char* address = nullptr, size_t connections = 32, size_t channels = 2, unsigned int down_speed = CONNECTION_SPEED_UNLIMITED, unsigned int up_speed = CONNECTION_SPEED_UNLIMITED);
	INLINE bool initClient(size_t connections = 1, size_t channels = 2, unsigned int down_speed = CONNECTION_SPEED_128K, unsigned int up_speed = CONNECTION_SPEED_128K);
	void destroy(void);

	INLINE void setBandwidthLimit(unsigned int down_speed, unsigned int up_speed);
	INLINE void setChannelLimit(size_t channels);

	INLINE unsigned int getHost(void) const;
	INLINE unsigned short getPort(void) const;

	// set compress/decompress functions used
	// INLINE void setCompression();

	INLINE Connection connect(const char* address, unsigned short port, size_t channels = 2);
	INLINE Connection getLatestConnection(void);

	void waitForEvent(NetworkEventCallback& callback, unsigned int timeout);
	void checkForEvent(NetworkEventCallback& callback);

	INLINE void broadcast(unsigned char channel, void* data, size_t data_size, unsigned int packet_flags = PACKET_RELIABLE);
	INLINE void flush(void);

	INLINE bool operator==(const Host& rhs) const;
	INLINE bool operator!=(const Host& rhs) const;

	// This shouldn't be called on host objects that have already been initialized
	const Host& operator=(const Host& rhs);

private:
	_ENetPeer* _latest_connection;
	_ENetHost* _host;

	friend class Connection;
};

NS_END
