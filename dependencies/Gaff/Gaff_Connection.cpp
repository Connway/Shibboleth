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

#include "Gaff_Connection.h"
#include "Gaff_IncludeAssert.h"
#include "Gaff_Host.h"
#include <enet/enet.h>

NS_GAFF

/*!
	\note Takes ownership of the connection.
*/
Connection::Connection(const Connection& rhs):
	_peer(rhs._peer), _cleanup(true)
{
	((Connection&)rhs)._peer = nullptr; // take over control of their _peer pointer
}

Connection::Connection(_ENetPeer* peer):
	_peer(peer), _cleanup(false)
{
}

Connection::Connection(void):
	_peer(nullptr), _cleanup(true)
{
}

Connection::~Connection(void)
{
	if (_cleanup) {
		destroy();
	}
}

/*!
	\brief Destroys the connection.
	\note Will call disconnectNow() if there is an active connection.
*/
void Connection::destroy(void)
{
	assert(_cleanup);

	if (_peer) {
		enet_peer_disconnect_now(_peer, 0);
		enet_peer_reset(_peer);
		_peer = nullptr;
	}
}

PeerIDType Connection::getID(void) const
{
	// Lazy way for giving a unique ID.
	// This is mainly used for the checkForEvent()/waitForEvent() callback
	// to identify which Connection the event came from.
	return reinterpret_cast<PeerIDType>(_peer);
}

unsigned int Connection::getHost(void) const
{
	assert(_peer);
	return _peer->address.host;
}

unsigned short Connection::getPort(void) const
{
	assert(_peer);
	return _peer->address.port;
}

void* Connection::getUserData(void) const
{
	assert(_peer);
	return _peer->data;
}

void Connection::setUserData(void* data)
{
	assert(_peer);
	_peer->data = data;
}

/*!
	\brief See the <a href="http://enet.bespin.org/group__peer.html#gab35807c848b6c88af12ce8476dffbc84">enet Documentation</a>.
*/
void Connection::setThrottle(unsigned int interval, unsigned int acceleration, unsigned int deceleration)
{
	assert(_peer);
	enet_peer_throttle_configure(_peer, interval, acceleration, deceleration);
}

/*!
	\brief See the <a href="http://enet.bespin.org/group__peer.html#gac48f35cdd39a89318a7b4fc19920b21b">enet Documentation</a>.
*/
void Connection::setTimeout(unsigned int limit, unsigned int min, unsigned int max)
{
	assert(_peer);
	enet_peer_timeout(_peer, limit, min, max);
}

/*!
	\brief Sets the interval at which a connection will be pinged. For more information see the <a href="http://enet.bespin.org/group__peer.html#gacddc2107f6e6b9e39812c1dfecff335b">enet Documentation</a>.
	\param interval Time in milliseconds to ping the connection.
*/
void Connection::setPingInterval(unsigned int interval)
{
	assert(_peer);
	enet_peer_ping_interval(_peer, interval);
}

/*!
	\brief See the <a href="http://enet.bespin.org/group__peer.html#ga0e807704b6ecace5004c2cdcfbf813c2">enet Documentation</a>.
*/
void Connection::disconnect(unsigned int data)
{
	assert(_peer);
	enet_peer_disconnect(_peer, data);
}

/*!
	\brief See the <a href="http://enet.bespin.org/group__peer.html#ga636cc45f52461b567d6daffe4ab8f4e9">enet Documentation</a>.
*/
void Connection::disconnectNow(unsigned int data)
{
	assert(_peer);
	enet_peer_disconnect_now(_peer, data);
}

/*!
	\brief See the <a href="http://enet.bespin.org/group__peer.html#ga759270d8cccec70f76274e93b49e5ac5">enet Documentation</a>.
*/
void Connection::disconnectLater(unsigned int data)
{
	assert(_peer);
	enet_peer_disconnect_later(_peer, data);
}

void Connection::ping(void)
{
	assert(_peer);
	enet_peer_ping(_peer);
}

/*!
	\brief Queues a packet to be sent.
	
	\param channel The channel to send the packet on.
	\param data The data we are sending.
	\param data_size The size of the data buffer (in bytes).
	\param packet_flags A list of bitwise or concatenated PacketFlags.
*/
bool Connection::send(unsigned char channel, void* data, size_t data_size, unsigned int packet_flags)
{
	assert(_peer);
	ENetPacket* packet = enet_packet_create(data, data_size, packet_flags);
	return !enet_peer_send(_peer, channel, packet);
}

bool Connection::valid(void) const
{
	return _peer != nullptr;
}

bool Connection::operator==(const Connection& rhs) const
{
	return _peer == rhs._peer;
}

bool Connection::operator!=(const Connection& rhs) const
{
	return _peer != rhs._peer;
}

const Connection& Connection::operator=(const Connection& rhs)
{
	_cleanup = rhs._cleanup;
	_peer = rhs._peer;

	if (_cleanup) {
		((Connection&)rhs)._peer = nullptr; // take over control of their _peer pointer
	}

	return *this;
}

Connection::operator bool(void) const
{
	return valid();
}

/*!
	\brief Connects to the \a address at \a port from the specified \a host.
	
	\param host The host we are connecting from.
	\param address The address to connect to.
	\param port The port to connect to.
	\param channels The number of channels to open on this connection.
	\param data User data to pass to the receiving host.
*/
void Connection::connect(Host& host, const char* address, unsigned short port, size_t channels, unsigned int data)
{
	ENetAddress addr = { ENET_HOST_ANY, port };

	if (enet_address_set_host(&addr, address)) {
		return;
	}

	_peer = enet_host_connect(host._host, &addr, channels, data);
}

NS_END
