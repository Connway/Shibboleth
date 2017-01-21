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

#include "Gaff_Host.h"
#include "Gaff_Assert.h"
#include <enet/enet.h>

NS_GAFF

/*!
	\brief Initializes the networking system. This is required to be called before any Connection or Host classes are instantiated.
*/
bool NetworkInit(
	NetworkAllocFunc alloc_func,
	NetworkFreeFunc free_func,
	NetworkNoMemoryFunc no_mem_func)
{
	GAFF_ASSERT((alloc_func == nullptr && free_func == nullptr) || (alloc_func != nullptr && free_func != nullptr));

	ENetCallbacks callbacks;
	callbacks.malloc = alloc_func;
	callbacks.free = free_func;
	callbacks.no_memory = no_mem_func;

	return !enet_initialize_with_callbacks(ENET_VERSION, &callbacks);
}

void NetworkDestroy(void)
{
	enet_deinitialize();
}

Host::Host(const Host& host):
	_latest_connection(nullptr), _host(host._host)
{
	const_cast<Host&>(host)._host = nullptr; // take over control of their _host pointer
}

Host::Host(void):
	_latest_connection(nullptr), _host(nullptr)
{
}

Host::~Host(void)
{
	destroy();
}

/*!
	\brief Initializes the Host as a server.

	\param port The port we will broadcast on.
	\param address The address we will bind to.
	\param connections The maximum number of incoming/outgoing connections.
	\param channels The number of channels allowed per connection.
	\param down_speed The maximum bandwidth allowed for incoming data.
	\param up_speed The maximum bandwidth allowed for outgoing data.

	\return Whether the server was successfully created.
*/
bool Host::initServer(unsigned short port, const char* address, size_t connections, size_t channels, unsigned int down_speed, unsigned int up_speed)
{
	GAFF_ASSERT(_host == nullptr);

	ENetAddress addr = { ENET_HOST_ANY, port };

	if (address) {
		if (enet_address_set_host(&addr, address)) {
			return false;
		}
	}

	_host = enet_host_create(&addr, connections, channels, down_speed, up_speed);
	return _host != nullptr;
}

/*!
	\brief Initializes the Host as a client.

	\param connections The maximum number of incoming/outgoing connections.
	\param channels The number of channels allowed per connection.
	\param down_speed The maximum bandwidth allowed for incoming data.
	\param up_speed The maximum bandwidth allowed for outgoing data.

	\return Whether the client was successfully created.
*/
bool Host::initClient(size_t connections, size_t channels, unsigned int down_speed, unsigned int up_speed)
{
	GAFF_ASSERT(_host == nullptr);
	_host = enet_host_create(nullptr, connections, channels, down_speed, up_speed);
	return _host != nullptr;
}

void Host::destroy(void)
{
	if (_host) {
		enet_host_destroy(_host);
		_latest_connection = nullptr;
		_host = nullptr;
	}
}

void Host::setBandwidthLimit(unsigned int down_speed, unsigned int up_speed)
{
	GAFF_ASSERT(_host);
	enet_host_bandwidth_limit(_host, down_speed, up_speed);
}

void Host::setChannelLimit(size_t channels)
{
	GAFF_ASSERT(_host);
	enet_host_channel_limit(_host, channels);
}

/*!
	\brief Get's the IP address encoded into an unsigned int.
*/
unsigned int Host::getHost(void) const
{
	GAFF_ASSERT(_host);
	return _host->address.host;
}

unsigned short Host::getPort(void) const
{
	GAFF_ASSERT(_host);
	return _host->address.port;
}

Connection Host::connect(const char* address, unsigned short port, size_t channels)
{
	GAFF_ASSERT(_host);
	Connection connection;
	connection.connect(*this, address, port, channels, 0);
	return connection;
}

/*!
	\brief Gets the latest connection generated from a EVENT_CONNECTION.
*/
Connection Host::getLatestConnection(void)
{
	Connection connection(_latest_connection);
	_latest_connection = nullptr;
	return connection;
}

/*!
	\brief Waits for a network event to occur and calls the \a callback when an event is generated.

	\param callback The callback we will call when an event occurs.
	\param timeout The number of milliseconds to wait for an event.
*/
void Host::waitForEvent(NetworkEventCallback& callback, unsigned int timeout)
{
	GAFF_ASSERT(_host);
	ENetEvent event;

	if (enet_host_service(_host, &event, timeout) > 0) {
		if (event.type == ENET_EVENT_TYPE_CONNECT) {
			_latest_connection = event.peer;
		}

		NetworkCallbackData data = {
			this, static_cast<NetworkEventType>(event.type),
			reinterpret_cast<PeerIDType>(event.peer), event.channelID,
			event.data, (event.packet) ? event.packet->data : nullptr,
			(event.packet) ? event.packet->dataLength : 0
		};

		callback(data);

		// Destroy the packet. This means callbacks that do stuff with the packet data
		// that want to retain that data for later will need to copy it in the callback.
		if (event.type == ENET_EVENT_TYPE_RECEIVE) {
			enet_packet_destroy(event.packet);
		}
	}
}

/*!
	\brief The same as waitForEvent(), but does not block the thread.
	\param callback The callback we will call when an event occurs.
*/
void Host::checkForEvent(NetworkEventCallback& callback)
{
	GAFF_ASSERT(_host);
	ENetEvent event;

	if (enet_host_check_events(_host, &event) > 0) {
		if (event.type == ENET_EVENT_TYPE_CONNECT) {
			_latest_connection = event.peer;
		}

		NetworkCallbackData data = {
			this, static_cast<NetworkEventType>(event.type),
			reinterpret_cast<PeerIDType>(event.peer), event.channelID,
			event.data, (event.packet) ? event.packet->data : nullptr,
			(event.packet) ? event.packet->dataLength : 0
		};

		callback(data);

		// Destroy the packet. This means callbacks that do stuff with the packet data
		// that want to retain that data for later will need to copy it in the callback.
		if (event.type == ENET_EVENT_TYPE_RECEIVE) {
			enet_packet_destroy(event.packet);
		}
	}
}

/*!
	\brief Broadcasts a packet to all connected peers.

	\param channel The channel to broadcast the packet on.
	\param data The packet data.
	\param data_size The packet size in bytes.
	\param packet_flags A list of bitwise or concatenated PacketFlags.
*/
void Host::broadcast(unsigned char channel, void* data, size_t data_size, unsigned int packet_flags)
{
	GAFF_ASSERT(_host);
	ENetPacket* packet = enet_packet_create(data, data_size, packet_flags);
	enet_host_broadcast(_host, channel, packet);
}

/*!
	\brief Flushes the packet queue.
*/
void Host::flush(void)
{
	GAFF_ASSERT(_host);
	enet_host_flush(_host);
}

bool Host::operator==(const Host& rhs) const
{
	return _host == rhs._host;
}

bool Host::operator!=(const Host& rhs) const
{
	return _host != rhs._host;
}

const Host& Host::operator=(const Host& rhs)
{
	GAFF_ASSERT(!_host);

	_host = rhs._host;
	((Host&)rhs)._host = nullptr; // take over control of their _host pointer

	return *this;
}

NS_END
