/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Gaff_Connection.h"

#define CONNECTION_SPEED_UNLIMITED 0
#define CONNECTION_SPEED_512K 65536
#define CONNECTION_SPEED_256K 32768
#define CONNECTION_SPEED_128K 16384
#define CONNECTION_SPEED_56K 7168

#define NETWORK_CALLBACK __cdecl

// Convenience function for the callback, cause damn that's a pain to type out
#define NETWORK_FUNCTION_TEMPLATE void, Gaff::Host&, Gaff::Host::EventType, Gaff::PeerIDType, unsigned char, unsigned int, void*, size_t
#define NETWORK_FUNCTION_IMPL Gaff::Host& host, Gaff::Host::EventType event_type, Gaff::PeerIDType peer_id, unsigned char channel, unsigned int user_data, void* data, size_t data_size

struct _ENetHost;
struct _ENetPeer;

NS_GAFF

bool NetworkInit(
	void* (NETWORK_CALLBACK *alloc)(size_t size) = nullptr,
	void (NETWORK_CALLBACK *free)(void* memory) = nullptr,
	void (NETWORK_CALLBACK *no_memory_callback)(void) = nullptr
);

void NetworkDestroy(void);

class Host
{
public:
	enum EventType
	{
		EVENT_NONE = 0,
		EVENT_CONNECTION,
		EVENT_DISCONNECT,
		EVENT_RECEIVED_PACKET
	};

	typedef IFunction<void, Host&, EventType, PeerIDType, unsigned char, unsigned int, void*, size_t> EventCallbackType;

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

	void waitForEvent(EventCallbackType& callback, unsigned int timeout);
	void checkForEvent(EventCallbackType& callback);

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
